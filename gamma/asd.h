#define CTRL(a) (a - '@')
#define CUR_TO_VIS 0
#define VIS_TO_CUR 1

#define SAFE 1
#define EXTND 1
#define INVOK 2
#define IMAGE 3
#define ESCAP 4
#define ANSWR 5
#define SOUND 6
#define BEEP 7
#define BACKS 8
#define FONT 9
#define LF 10
#define CHATR 11
#define FF 12
#define CR 13
#define POSCR 14
#define INSDL 15
#define BOX 16
#define CURSR 17
#define MASS 18
#define VERTR 19
#define HORIR 20
#define HORRP 21
#define VERRP 22
#define BORDR 23
#define ATTRC 24
#define REPET 25
/* REPET is a fixup for total ANSI (TheDraw & ANSI2ASD) compliance... */
#define PAGES 26
#define LSCRL 28
#define GASDS 29

int ASDInit(char adapter);
short ASDDeInit(void);
void asend(unsigned char inchar);
unsigned short agetch(void);
short akbhit(void);
void receiveASD(unsigned short (*astdin)(void),
                unsigned short (*stdstat)(void));
void asends(unsigned char *instring);
void asendc(unsigned char *instring, short count);
void addcrlf(unsigned char inchar);
void baseASD(unsigned char inchar);
void ASDwait(void (*fn_action)(), unsigned char waitfor);
void ASDgetargs(unsigned char inchar);
void respondASD(void (*newres)(unsigned char));
void escapASD(void);
void chatrASD(void);
void extndASD(void);
void poscrASD(void);
void boxASD(void);
void cursrASD(void);
void vertrASD(void);
void horirASD(void);
void horrpASD(void);
void imageASD(unsigned char inchar);
void getimageASD(void);
void putimageASD(void);
void insdlASD(void);
void lscrlASD(void);
void resetoneASD(void);
void bordrASD(void);
void verrpASD(void);
void attrcASD(void);
void soundASD(void);
void repetASD(void);
void pagesASD(void);
void cdecl aprintf(unsigned char *constr, ...);
void comp_output(unsigned char inchar);
void comp_flush(void);
void _format(void (*outptr)(unsigned char), unsigned char *inptr, char *vargs);
short newstdoutASD(void (*filtered)(unsigned char),
                   void (*unfiltered)(unsigned char));
void resetASD(void);
void sBox(unsigned short mask, unsigned char width, unsigned char height,
          unsigned char filler);
void sGetImage(unsigned char width, unsigned char height, unsigned char buffr);
void sPutImage(unsigned char buffr);
void sEscape(unsigned char escchar);
void sChangeAttr(unsigned char newattr);
void sPositionCur(unsigned char newx, unsigned char newy);
void sCurShape(unsigned char shape);
void sCurControl(unsigned char action);
void sCurUp(unsigned char count);
void sCurDown(unsigned char count);
void sCurBack(unsigned char count);
void sCurForward(unsigned char count);
void sBorder(unsigned char width, unsigned char height, unsigned char style);
void sChangeBar(unsigned char color, unsigned char run);
void sScrollLeft(unsigned char num);
void sScrollRight(unsigned char num);
void sFreeHandle(unsigned char handle);
void sRepeatChar(unsigned char rchar, unsigned char nrep);
void sLineEdit(unsigned char action);
void sPageSelect(unsigned char pagenum);
void sPageCopy(unsigned char to_page);
void sVerLine(unsigned char vector, unsigned char inchar);
void sImageReset(void);
void getreceiveASD(unsigned short (**astdin)(void),
                   unsigned short (**stdstat)(void));
void getstdoutASD(void (**filtered)(unsigned char),
                  void (**unfiltered)(unsigned char));
void cdecl v_printf(unsigned char *constr, ...);
