
option expr32

; Begin of file video.h
LBRACK		EQU		27t
BKSLSH		EQU		28t
RBRACK		EQU		29t
INSERT		EQU		30t
HOME		EQU		31t
PGUP		EQU		32t
DELETE		EQU		33t
KEND		EQU		34t
PGDOWN		EQU		35t
UPARROW		EQU		36t
LEFTARROW		EQU		37t
DOWNARROW		EQU		38t
RIGHTARROW		EQU		39t
GREYSTAR		EQU		253t
GREYMINUS		EQU		254t
GREYPLUS		EQU		255t
BLK		EQU		0t
BLU		EQU		1t
GRN		EQU		2t
CYA		EQU		3t
RED		EQU		4t
PUR		EQU		5t
BRN		EQU		6t
WHT		EQU		7t
HBLK		EQU		8t
HBLU		EQU		9t
HGRN		EQU		10t
HCYA		EQU		11t
HRED		EQU		12t
HPUR		EQU		13t
HBRN		EQU		14t
HWHT		EQU		15t
CHARMASK		EQU		0000000ffh
ATTRMASK		EQU		00000ff00h
CNSAV		EQU		16t
NUMPAGES		EQU		2t
NUM_IM		EQU		32t
DEFAULT_ATTR		EQU		7t
IM_MEMTOTAL		EQU		12000t
FINDADAPTER		EQU		0t
MDA		EQU		1t
CGA		EQU		2t
EGA		EQU		3t
MCGA		EQU		4t
VGA		EQU		5t
HGC		EQU		000000080h
HGCPlus		EQU		000000081h
InColor		EQU		000000082h
MDADisplay		EQU		1t
CGADisplay		EQU		2t
EGAColorDisplay		EQU		3t
PS2MonoDisplay		EQU		4t
PS2ColorDisplay		EQU		5t
ADAPTER_TYPES		EQU		5t
CURSOR_SIZES		EQU		6t
HIGHBIT		EQU		128t
BORDER_TYPES		EQU		11t
CUR_HIDDEN		EQU		0t
CUR_OVER		EQU		1t
CUR_HALF_OVER		EQU		2t
CUR_FULL		EQU		3t
CUR_HALF_UNDER		EQU		4t
CUR_UNDER		EQU		5t
CUR_SAVE		EQU		6t
CUR_RESTORE		EQU		7t
CUR_NOUPDATE		EQU		8t
CUR_UPDATE		EQU		9t
DEFAULT_CURSOR		EQU		000000005h
VNOMEM		EQU		1t
VOK		EQU		0t
BLON		EQU		1t
BLOFF		EQU		0t
NO_UPDATE		EQU		0t
YES_UPDATE		EQU		1t
@proto_0		TYPEDEF		PROTO C :PTR VIDhandle
VideoID		PROTO		@proto_0

@proto_1		TYPEDEF		PROTO C :WORD, :BYTE, :BYTE, :BYTE
vBox		PROTO		@proto_1

@proto_2		TYPEDEF		PROTO C :BYTE
vPutch		PROTO		@proto_2

@proto_3		TYPEDEF		PROTO C 
vCupDat		PROTO		@proto_3

@proto_4		TYPEDEF		PROTO C :BYTE, :BYTE, :BYTE
vGetImage		PROTO		@proto_4

@proto_5		TYPEDEF		PROTO C :BYTE
vPutImage		PROTO		@proto_5

@proto_6		TYPEDEF		PROTO C :SBYTE
vInit		PROTO		@proto_6

@proto_7		TYPEDEF		PROTO C 
vDeInit		PROTO		@proto_7

@proto_8		TYPEDEF		PROTO C :SBYTE
vBlinkControl		PROTO		@proto_8

@proto_9		TYPEDEF		PROTO C 
vScrollUp		PROTO		@proto_9

@proto_10		TYPEDEF		PROTO C :PTR BYTE
vPuts		PROTO		@proto_10

@proto_11		TYPEDEF		PROTO C :BYTE
vCursorControl		PROTO		@proto_11

@proto_12		TYPEDEF		PROTO C 
vClearScreen		PROTO		@proto_12

@proto_13		TYPEDEF		PROTO C :BYTE
vChangeAttr		PROTO		@proto_13

@proto_14		TYPEDEF		PROTO C :BYTE, :BYTE
vPosCur		PROTO		@proto_14

@proto_15		TYPEDEF		PROTO C 
vCR		PROTO		@proto_15

@proto_16		TYPEDEF		PROTO C 
vLF		PROTO		@proto_16

@proto_17		TYPEDEF		PROTO C 
vBACKS		PROTO		@proto_17

@proto_18		TYPEDEF		PROTO C :BYTE
vHorizRel		PROTO		@proto_18

@proto_19		TYPEDEF		PROTO C :BYTE
vVertRel		PROTO		@proto_19

@proto_20		TYPEDEF		PROTO C :BYTE, :BYTE
vRepeatHVC		PROTO		@proto_20

@proto_21		TYPEDEF		PROTO C 
vResetImage		PROTO		@proto_21

@proto_22		TYPEDEF		PROTO C :BYTE
vResetOne		PROTO		@proto_22

@proto_23		TYPEDEF		PROTO C 
vCurLineToScrap		PROTO		@proto_23

@proto_24		TYPEDEF		PROTO C 
vScrapToCurLine		PROTO		@proto_24

@proto_25		TYPEDEF		PROTO C 
vClearCurLine		PROTO		@proto_25

@proto_26		TYPEDEF		PROTO C 
vDelCurLine		PROTO		@proto_26

@proto_27		TYPEDEF		PROTO C 
vInsertCurLine		PROTO		@proto_27

@proto_28		TYPEDEF		PROTO C :BYTE
vLineEdit		PROTO		@proto_28

@proto_29		TYPEDEF		PROTO C 
vInsertBlank		PROTO		@proto_29

@proto_30		TYPEDEF		PROTO C 
vDeleteCur		PROTO		@proto_30

@proto_31		TYPEDEF		PROTO C :BYTE, :BYTE, :BYTE
vBorder		PROTO		@proto_31

@proto_32		TYPEDEF		PROTO C :BYTE, :BYTE
vRepeatVVC		PROTO		@proto_32

@proto_33		TYPEDEF		PROTO C :PTR SBYTE, :SBYTE, :BYTE, :SBYTE
vStatLine		PROTO		@proto_33

@proto_34		TYPEDEF		PROTO C 
vClearEOL		PROTO		@proto_34

@proto_35		TYPEDEF		PROTO C :BYTE, :BYTE
vRepeat		PROTO		@proto_35

@proto_36		TYPEDEF		PROTO C :BYTE
vCurType		PROTO		@proto_36

@proto_37		TYPEDEF		PROTO C 
vGetHandle		PROTO		@proto_37

@proto_38		TYPEDEF		PROTO C :BYTE, :BYTE
vAttrChange		PROTO		@proto_38

@proto_39		TYPEDEF		PROTO C :BYTE
vPage		PROTO		@proto_39

@proto_40		TYPEDEF		PROTO C :BYTE
vLineScroll		PROTO		@proto_40

@proto_41		TYPEDEF		PROTO C :PTR BYTE, :PTR BYTE
vWidthHeight		PROTO		@proto_41

@proto_42		TYPEDEF		PROTO C 
vGetch		PROTO		@proto_42

@proto_43		TYPEDEF		PROTO C 
vKbhit		PROTO		@proto_43

@proto_44		TYPEDEF		PROTO C :WORD
vmode		PROTO		@proto_44

@proto_45		TYPEDEF		PROTO C 
vGetAttr		PROTO		@proto_45

@proto_46		TYPEDEF		PROTO C 
vReset		PROTO		@proto_46

; End of file video.h
