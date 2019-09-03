.MODEL small, c

INCLUDE extra.inc
INCLUDE video.inc
INCLUDE videodat.inc

; padding of 2's
SIZEIMAGE EQU SIZEOF imagespecs + (SIZEOF imagespecs) MOD 2
EXTERN vbase:vcontrol
@myproto	TYPEDEF		PROTO C :VARARG
printf		PROTO		@myproto

.DATA

EGADisplays	BYTE	CGADisplay	; 0000b, 0001b	(EGA switch values)
		BYTE	EGAColorDisplay	; 0010b, 0011b
		BYTE	MDADisplay	; 0100b, 0101b
		BYTE	CGADisplay	; 0110b, 0111b
		BYTE	EGAColorDisplay	; 1000b, 1001b
		BYTE	MDADisplay	; 1010b, 1011b
DCCtable	BYTE	0,0		; translate table for INT 10h func 1Ah
		BYTE	MDA,MDADisplay
		BYTE	CGA,CGADisplay
		BYTE	0,0
		BYTE	EGA,EGAColorDisplay
		BYTE	EGA,MDADisplay
		BYTE	0,0
		BYTE	VGA,PS2MonoDisplay
		BYTE	VGA,PS2ColorDisplay
		BYTE	0,0
		BYTE	MCGA,EGAColorDisplay
		BYTE	MCGA,PS2MonoDisplay
		BYTE	MCGA,PS2ColorDisplay
TestSequence    BYTE	TRUE            ; this list of flags and addresses
		WORD	FindPS2		;  determines the order in which this
EGAflag         BYTE	?               ;  subsystems
		WORD	FindEGA
CGAflag         BYTE	?
		WORD	FindCGA
Monoflag        BYTE	?
		WORD	FindMono
NumberOfTests	EQU	($-TestSequence)/3

.CODE

vBox	PROC USES di si ds, smask:WORD, char:BYTE, swidth:BYTE, height:BYTE
	xor	ax, ax
	mov	al, height
	mov	word ptr height, ax
	mov	al, swidth
	mov	word ptr swidth, ax
	add	ax, vbase.pages.writepos.crsx
	mov	bx, vbase.pages.swidth
	sub	ax, bx
	js	thubfwidth
	sub	word ptr swidth, ax
thubfwidth:
	mov	ax, word ptr height
	add	ax, vbase.pages.writepos.crsy
	mov	bx, vbase.pages.height
	sub	ax, bx
	js	thubfheight
	sub	word ptr height, ax
thubfheight:
	mov	dx, smask
	mov	cx, word ptr swidth
	mov	bx, vbase.pages.swidth
	sub	bx, cx
	shl	bx, 1
	mov	smask, bx	; mask becomes yadd
	mov	ch, byte ptr height

	mov	bl, char
	mov	bh, byte ptr vbase.pages.curattr
	and	bx, dx
	not	dx
	
	mov	ax, vbase.pages.writepos.idx
	shl	ax, 1
	les	di, vbase.pages.pagemem
	add	di, ax				; load DI, AX is used
loophere:
	mov	ax, es:[di]
	and	ax, dx
	or	ax, bx
	stosw
	dec	cl
	jnz	loophere
	dec	ch
	jz	okleave
	add	di, smask
	mov	cl, byte ptr swidth
	jmp	loophere
okleave:
	ret
vBox	ENDP

vCupDat	PROC USES si ds
	mov	ax, vbase.pages.crsupdat	; update cursor position?
	or	ax, ax
	jz	scupscram
	mov	bx, 0e0fh
	mov	dx, vbase.pages.CRTC	; get the CRTC port from vbase
	mov	cx, vbase.pages.writepos.idx	; get offset in CX
	mov	al, bh		; (Cursor location high register number)
	out	dx, al
	inc	dx		; dx = data register port
	mov	al, ch
	out	dx, al		; output high byte to CRTC
	dec	dx		; point to CRTC register port now
	mov	al, bl		; set to Cursor Location High
	out	dx, al
	inc	dx		; DX = data port
	mov	al, cl
	out	dx, al
scupscram:
	ret
vCupDat	ENDP

vGetImage PROC USES di si ds, xsize:BYTE, ysize:BYTE, box:BYTE
	LOCAL	idxfrom:WORD, idxdata:WORD
	mov	bl, box
	or	bl, bl
	jnz	get_ok
	jmp	get_leave
get_ok:
	cmp	bl, NUM_IM
	jb	get_ok_again
	jmp	get_leave
get_ok_again:

	mov	ax, vbase.pages.writepos.idx
	shl	ax, 1
	mov	idxfrom, ax	; idxfrom = offset into screen memory

	xor	bx, bx
	mov	bl, ysize
	mov	word ptr ysize, bx
	mov	bl, xsize
	mov	word ptr xsize, bx
	mov	ax, vbase.pages.writepos.crsx
	add	ax, bx
	sub	ax, vbase.pages.swidth
	jb	get_width
	sub	word ptr xsize, ax
get_width:

	mov	bx, word ptr ysize
	mov	ax, vbase.pages.writepos.crsy
	add	ax, bx
	sub	ax, vbase.pages.height
	jb	get_height
	sub	word ptr ysize, ax
get_height:

	mov	al, box		; set bx as base for vbase.images[box]
	dec	al
	mov	bl, SIZEIMAGE
	mul	bl
	mov	bx, ax
	add	bx, OFFSET ds:vbase.images

	mov	cx, [bx].imagespecs.iused	; box unused
	or	cx, cx
	jz	get_unused

	mov	cx, [bx].imagespecs.iwidth	; xsize same?
	cmp	cx, word ptr xsize
	jnz	get_bad_size

	mov	cx, [bx].imagespecs.iheight	; ysize same?
	cmp	cx, word ptr ysize
	jnz	get_bad_size

	mov	ax, [bx].imagespecs.idata	; both same, change nothing
	mov	idxdata, ax			; NOT! scaled!
	jmp	get_go
get_bad_size:			; AX = idxto
	push	ds
	push	bx
	INVOKE	vResetOne, box
	mov	idxdata, ax
	pop	bx
	pop	ds
	jmp	get_common
get_unused:				; AX = idxto
	mov	ax, IM_MEMTOTAL/2
	sub	ax, vbase.imemleft
	mov	idxdata, ax
get_common:

	mov	ax, word ptr xsize		; create 
	mul	BYTE PTR ysize
	cmp	ax, vbase.imemleft
	ja	get_leave		; not enough memory left for image
	sub	vbase.imemleft, ax	; update vbase.images[box] structure

	mov	ax, word ptr xsize
	mov	[bx].imagespecs.iwidth, ax
	mov	ax, word ptr ysize
	mov	[bx].imagespecs.iheight, ax

	mov	ax, idxdata
	mov	[bx].imagespecs.idata, ax
	mov	[bx].imagespecs.iused, 1

get_go:
	mov	dx, vbase.pages.swidth
	sub	dx, word ptr xsize
	shl	dx, 1			; DX = yadd
	mov	ax, idxdata
	shl	ax, 1			; idata must be scaled too!
	les	di, vbase.imagemem	; ES:DI = destination

	add	di, ax
	mov	bx, word ptr ysize	; BX = lines left
	mov	ax, word ptr xsize	; AX = repetitions

	ASSUME	ds:nothing
	lds	si, vbase.pages.pagemem
	add	si, idxfrom
	pushf
	cld
get_back_here:
	mov	cx, ax
	rep	movsw
	add	si, dx
	dec	bx
	jnz	get_back_here
	popf
get_leave:
	ret
vGetImage ENDP


vPutImage PROC USES si di ds, box:BYTE
	LOCAL	xcount:WORD, ysplus:WORD, ydplus:WORD, yto:WORD
	mov	bl, box
	or	bl, bl
	jnz	put_ok
	jmp	put_leave
put_ok:
	cmp	bl, NUM_IM
	jb	put_ok_again
	jmp	put_leave
put_ok_again:
	mov	al, box		; set bx as base for vbase.images[box]
	dec	al
	mov	bl, SIZEIMAGE
	mul	bl
	mov	bx, ax
	add	bx, OFFSET ds:vbase.images

	mov	cx, [bx].imagespecs.iused	; box unused
	or	cx, cx
	jz	put_leave
	
	mov	ax, [bx].imagespecs.iwidth
	mov	cx, ax					; cx = OLD XSIZE
	mov	dx, ax					; ax = temp X
	xor	ax, ax
	add	ax, vbase.pages.writepos.crsx	; dx = TRIMMED X
	sub	ax, vbase.pages.swidth
	jb	put_width
	sub	dx, ax
put_width:
	sub	cx, dx
	mov	xcount, dx
	shl	cx, 1
	mov	ysplus, cx

	mov	ax, vbase.pages.swidth
	sub	ax, dx
	shl	ax, 1
	mov	ydplus, ax

	mov	cx, [bx].imagespecs.iheight
	mov	yto, cx
	mov	ax, vbase.pages.writepos.crsy
	add	ax, cx
	sub	ax, vbase.pages.height
	jb	put_height
	sub	yto, ax
put_height:
		; now have X & Y trimmed to screen size; ysplus set

	mov	ax, vbase.pages.writepos.idx	; set screen dest
	shl	ax, 1
	les	di, vbase.pages.pagemem
	add	di, ax

	mov	ax, [bx].imagespecs.idata		; set up source
	shl	ax, 1
	lds	si, vbase.imagemem
	ASSUME	ds:NOTHING
	add	si, ax

	mov	dx, yto			; DX = lines left
	mov	ax, ydplus		; AX = swidth - xsize
	mov	bx, ysplus		; BX = image not displayed
	pushf
	cld
put_back_here:
	mov	cx, xcount
	rep	movsw
	add	si, bx
	add	di, ax
	dec	dx
	jnz	put_back_here
	popf
put_leave:
	ret
vPutImage ENDP

vPutch PROC uses di tchar:BYTE
	mov	cx, vbase.pages.swidth
	les	di, vbase.pages.pagemem
	mov	bx, vbase.pages.writepos.idx
	mov	ah, byte ptr vbase.pages.curattr
	mov	al, tchar
	mov	dx, vbase.pages.writepos.crsx
	shl	bx, 1
	mov	es:[di][bx], ax
	shr	bx, 1

	inc	bx
	cmp	bx, vbase.pages.numidx
	jae	putc_scroll
	inc	dx
	cmp	dx, cx
	jb	putc_notnext
	inc	vbase.pages.writepos.crsy
	xor	dx, dx
putc_notnext:
	mov	vbase.pages.writepos.crsx, dx
	mov	vbase.pages.writepos.idx, bx
	INVOKE	vCupDat
	ret
putc_scroll:
	sub	bx, cx		; scroll here
	xor	dx, dx
	mov	vbase.pages.writepos.crsx, dx
	mov	vbase.pages.writepos.idx, bx

	INVOKE	vScrollUp
	INVOKE	vCupDat
	ret
vPutch	ENDP	

VideoID	PROC USES si di ds, Devices:PTR VIDhandle
	ASSUME	ds:DGROUP
	IF	@DataSize
		les	di, Devices
		mov	ax, DGROUP
		mov	ds, ax
		REFVID	EQU es:[di].VIDhandle
	ELSE
		mov	di, Devices	; ES:DI -> start of data structure
		REFVID	EQU [di].VIDhandle
	ENDIF
	mov	WORD PTR REFVID.Video0Type, 0	; zero these variables
	mov	WORD PTR REFVID.Video1Type, 0
	mov     CGAflag, TRUE
	mov     EGAflag, TRUE
	mov     Monoflag,TRUE
	mov	cx, NumberOfTests
	mov	si, offset TestSequence
L01:	lodsb			; AL := flag
	test	al, al
	lodsw			; AX := subroutine address
	jz	L02		; skip subroutine if flag is false
	push	si
	push	cx
	call	ax		; call subroutine to detect subsystem
	pop	cx
	pop	si
L02:	loop	L01
	call	FindActive
	ret
VideoID	ENDP

FindPS2	PROC	near
	mov	ax, 1A00h
	int	10h		; call video BIOS for info
	cmp	al, 1Ah
	jne	L13		; exit if function not supported (i.e.,
	mov	cx, bx
	xor	bh, bh		; BX := DCC for active subsystem
	or	ch, ch
	jz	L11		; jump if only one subsystem present
	mov	bl, ch		; BX := inactive DCC
	add	bx, bx
	mov	ax, [bx + offset DCCtable]
	mov	WORD PTR REFVID.Video1Type, ax
	mov	bl, cl
	xor	bh, bh		; BX := active DCC
L11:	add	bx, bx
	mov	ax, [bx + offset DCCtable]
	mov	WORD PTR REFVID.Video0Type, ax
	mov	CGAflag, FALSE
	mov	EGAflag, FALSE
	mov	Monoflag,FALSE
	lea	bx, REFVID.Video0Type  ; if the BIOS reported an MDA ...
	cmp	BYTE PTR [bx], MDA
	je	L12
	lea	bx, REFVID.Video1Type
	cmp	BYTE PTR [bx], MDA
	jne	L13
L12:	mov	WORD PTR [bx], 0    ; ... Hercules can't be ruled out
	mov	Monoflag, TRUE
L13:	ret
FindPS2	ENDP

FindEGA		PROC	near		; Caller:	AH = flags
	mov	bl, 10h		; BL := 10h (return EGA info)
	mov	ah, 12h		; AH := INT 10H function number
	int	10h		; call EGA BIOS for info
	cmp	bl, 10h
	je	L22		; jump if EGA BIOS not present
	mov	al, cl
	shr	al, 1		; AL := switches/2
	mov	bx, offset EGADisplays
	xlat			; determine display type from switches
	mov	ah, al		; AH := display type
	mov	al, EGA		; AL := subystem type
	call	FoundDevice
	cmp	ah, MDADisplay
	je	L21		; jump if EGA has a monochrome display
	mov	CGAflag, FALSE	; no CGA if EGA has color display
	jmp	L22
L21:	mov	Monoflag, FALSE	; EGA has a mono display, so MDA and
L22:	ret
FindEGA		ENDP

FindCGA		PROC	near	; Returns:	VIDhandle updated
	mov	dx, 3D4h	; DX := CRTC address port
	call	Find6845
	jc	L31		; jump if not present
	mov	al, CGA
	mov	ah, CGADisplay
	call	FoundDevice
L31:	ret
FindCGA		ENDP

FindMono	PROC	near	; Returns:	VIDhandle updated
	mov	dx, 3B4h 	; DX := CRTC address port
	call	Find6845
	jc	L44		; jump if not present
	mov	dl, 0BAh 	; DX := 3BAh (status port)
	in	al, dx
	and	al, 80h
	mov	ah, al		; AH := bit 7 (vertical sync on HGC)
	mov	cx, 8000h	; do this 32768 times 
L41:	in	al, dx
	and	al, 80h		; isolate bit 7
	cmp	ah, al
	loope	L41		; wait for bit 7 to change
	jne	L42		; if bit 7 changed, it's a Hercules
	mov	al, MDA		; if bit 7 didn't change, it's an MDA
	mov	ah, MDADisplay
	call	FoundDevice
	jmp	L44
L42:	in	al, dx
	mov	dl, al		; DL := value from status port
	mov	ah, MDADisplay	; assume it's a monochrome display
	mov	al, HGC		; look for an HGC
	and	dl, 01110000b	; mask off bits 4 thru 6
	jz	L43
	mov	al, HGCPlus	; look for an HGC+
	cmp	dl, 00010000b
	je	L43		; jump if it's an HGC+
	mov	al, InColor	; it's an InColor card
	mov	ah, EGAColorDisplay
L43:	call	FoundDevice
L44:	ret
FindMono	ENDP

Find6845	PROC	near		; Caller:	DX = port addr
	mov	al, 0Fh
	out	dx, al		; select 6845 reg 0Fh (Cursor Low)
	inc	dx
	in	al, dx		; AL := current Cursor Low value
	mov	ah, al		; preserve in AH
	mov	al, 66h		; AL := arbitrary value
	out	dx, al		; try to write to 6845
	mov	cx, 100h
L51:	loop	L51		; wait for 6845 to respond
	in	al, dx
	xchg	ah, al		; AH := returned value
	out	dx, al		; restore original value
	cmp	ah, 66h		; test whether 6845 responded
	je	L52		; jump if it did (cf is reset)
	stc			; set carry flag if no 6845 present
L52:	ret	
Find6845	ENDP

FindActive	PROC	near
	cmp	WORD PTR REFVID.Video0Type, 0
	je	L63			; exit if only one subsystem
	cmp	REFVID.Video0Type, 4
	jge	L63			;  (INT 10H function 1AH
	cmp	REFVID.Video1Type, 4	;  already did the work)
	jge	L63
	mov	ah, 0Fh
	int	10h			; AL := current BIOS video mode
	and	al, 7
	cmp	al, 7			; jump if monochrome
	je	L61			;  (mode 7 or 0Fh)
	cmp	REFVID.Display0Type, MDADisplay
	jne	L63			; exit if Display0 is color
	jmp	short L62
L61:	cmp	REFVID.Display0Type, MDADisplay
	je	L63			; exit if Display0 is monochrome
L62:	mov	ax, WORD PTR REFVID.Video0Type
	xchg	ax, WORD PTR REFVID.Video1Type
	mov	WORD PTR REFVID.Video0Type, ax
L63:	ret
FindActive	ENDP

FoundDevice	PROC	near			; Caller:    AH = display #
	lea	bx, REFVID.Video0Type
	cmp	byte ptr [bx], 0
	je	L71			; jump if 1st subsystem
	lea	bx, REFVID.Video1Type	; must be 2nd subsystem
L71:	mov	[bx], ax			; update list entry
	ret
FoundDevice	ENDP

END
