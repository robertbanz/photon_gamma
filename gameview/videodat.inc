
option expr32

; Begin of file videodat.h
NUM_IM		EQU		32t
CNSAV		EQU		16t
@tag_0		STRUCT 2t
idx		WORD		?
crsx		WORD		?
crsy		WORD		?
@tag_0		ENDS

cposition		TYPEDEF		@tag_0

@tag_1		STRUCT 2t
idx		WORD		?
crsx		WORD		?
crsy		WORD		?
ctype		WORD		?
@tag_1		ENDS

csavedpos		TYPEDEF		@tag_1

@t_0		TYPEDEF		FAR PTR SWORD
@tag_2		STRUCT 2t
pagemem		@t_0		?
storedidx		WORD		?
curattr		WORD		?
crsupdat		WORD		?
curtype		WORD		?
writepos		cposition		<>
crspos		cposition		<>
stored		csavedpos		16t DUP (<>)
@tag_2		ENDS

vpagestuff		TYPEDEF		@tag_2

@tag_3		STRUCT 2t
idata		WORD		?
iwidth		WORD		?
iheight		WORD		?
iused		WORD		?
@tag_3		ENDS

imagespecs		TYPEDEF		@tag_3

@t_1		TYPEDEF		FAR PTR SWORD
@t_2		TYPEDEF		FAR PTR SWORD
@t_3		TYPEDEF		FAR PTR SWORD
vcontrol		STRUCT 2t
pages		vpagestuff		<>
numidx		WORD		?
CRTC		WORD		?
imemleft		WORD		?
height		WORD		?
swidth		WORD		?
curpage		WORD		?
textmode		WORD		?
cardref		WORD		?
imagemem		@t_1		?
linemem		@t_2		?
vrealmem		@t_3		?
images		imagespecs		32t DUP (<>)
vcontrol		ENDS

VIDhandle		STRUCT 2t
Video0Type		BYTE		?
Display0Type		BYTE		?
Video1Type		BYTE		?
Display1Type		BYTE		?
VIDhandle		ENDS

; End of file videodat.h
