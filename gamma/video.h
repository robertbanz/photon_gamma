#define COLOR(forground, background) \
  (unsigned char)(forground | (background << 4))
#define check(a) vStatLine(a, 0, COLOR(HWHT, HBLK), 1);

/******************** extended key definitions *******************/
#define LBRACK 27
#define BKSLSH 28
#define RBRACK 29
#define INSERT 30
#define HOME 31
#define PGUP 32
#define DELETE 33
#define KEND 34
#define PGDOWN 35
#define UPARROW 36
#define LEFTARROW 37
#define DOWNARROW 38
#define RIGHTARROW 39
/***************** these key codes return a 0 first *****************/
#define GREYSTAR 253
#define GREYMINUS 254
#define GREYPLUS 255
/*
#define KEY7 245
#define KEY8 246
#define KEY9 247
#define KEY4 248
#define KEY5 249
#define KEY6 250
#define KEY1 251
#define KEY2 252
#define KEY3 253
#define KEY0 254
#define KEYDOT 255
*/
/************HERC COLORS**********/
#define hrcBLACK 0x00
#define hrcNORM 0x07
#define hrcREV 0x70
#define hrcUNDR 0x01
#define hrcWHITE 0x77
#define hrcBLINK 0x80
#define hrcINTEN 0x08

#define BLK 0
#define BLU 1
#define GRN 2
#define CYA 3
#define RED 4
#define PUR 5
#define BRN 6
#define WHT 7
#define HBLK 8
#define HBLU 9
#define HGRN 10
#define HCYA 11
#define HRED 12
#define HPUR 13
#define HBRN 14
#define HWHT 15

#define CHARMASK 0x00ff
#define ATTRMASK 0xff00
#define CNSAV 16
#define NUMPAGES 2
#define NUM_IM 32
#define DEFAULT_ATTR 7
#define IM_MEMTOTAL 12000
#define CO40 0x10
#define FINDADAPTER 0
#define MDA 0x01
#define CGA 0x02
#define EGA 0x03
#define MCGA 0x04
#define VGA 0x05
#define HGC 0x80
#define HGCPlus 0x81
#define InColor 0x82
#define MDADisplay 1
#define CGADisplay 2
#define EGAColorDisplay 3
#define PS2MonoDisplay 4
#define PS2ColorDisplay 5
#define ADAPTER_TYPES 5
#define CURSOR_SIZES 6
#define HIGHBIT 128
#define BORDER_TYPES 11
#define CUR_HIDDEN 0
#define CUR_OVER 1
#define CUR_HALF_OVER 2
#define CUR_FULL 3
#define CUR_HALF_UNDER 4
#define CUR_UNDER 5
#define CUR_SAVE 6
#define CUR_RESTORE 7
#define CUR_NOUPDATE 8
#define CUR_UPDATE 9
#define DEFAULT_CURSOR CUR_UNDER

#define VNOMEM 1
#define VOK 0
#define BLON 1
#define BLOFF 0
#define NO_UPDATE 0
#define YES_UPDATE 1

/* Assembly routines */
void VideoID(struct VIDhandle *);
void vBox(unsigned short mask, unsigned char filler, unsigned char swidth,
          unsigned char height);
void vPutch(unsigned char tchar);
void vCupDat(void);
void vGetImage(unsigned char xsize, unsigned char ysize, unsigned char box);
void vPutImage(unsigned char box);

/* C routines */
int vInit(char adapter);
int vDeInit(void);
void vBlinkControl(char onoff);
void vScrollUp(void);
void vPuts(unsigned char *tstring);
void vCursorControl(unsigned char curact);
void vClearScreen(void);
void vChangeAttr(unsigned char nattr);
void vPosCur(unsigned char nx, unsigned char ny);
void vCR(void);
void vLF(void);
void vBACKS(void);
void vHorizRel(unsigned char vector);
void vVertRel(unsigned char vector);
void vRepeatHVC(unsigned char rchar, unsigned char vector);
void vResetImage(void);
int vResetOne(unsigned char box);
void vCurLineToScrap(void);
void vScrapToCurLine(void);
void vClearCurLine(void);
void vDelCurLine(void);
void vInsertCurLine(void);
void vLineEdit(unsigned char lfunc);
void vInsertBlank(void);
void vDeleteCur(void);
void vBorder(unsigned char xsize, unsigned char ysize, unsigned char bstyle);
void vRepeatVVC(unsigned char vector, unsigned char rchar);
void vStatLine(char *message, char position, unsigned char color, char cleol);
void vClearEOL(void);
void vRepeat(unsigned char rchar, unsigned char vector);
void vCurType(unsigned char newtype);
unsigned char vGetHandle(void);
void vAttrChange(unsigned char color, unsigned char runlength);
void vPage(unsigned char page);
void vLineScroll(unsigned char vector);
void vWidthHeight(unsigned char *swidth, unsigned char *height);
unsigned short vGetch(void);
unsigned short vKbhit(void);
void vmode(unsigned short mode);
unsigned char vGetAttr(void);
void vReset(void);
void _cdecl v_printf(unsigned char *constr, ...);
void v_sends(unsigned char *in);
char _far *vPageMem(int page);
