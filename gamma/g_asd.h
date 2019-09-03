/***********************************************************************
    Graphical ASD calls

    ASD (c) 1991,92,93 Chris Fanning
    Graphics calls (c) 1993 Robert Banz
************************************************************************/

#define GASD_SWTCH2GRFX 1
#define GASD_SWTCH2TEXT 2
#define GASD_SETFONT 3
#define GASD_NEWLINE 4
#define GASD_CLEAR 5
#define GASD_COLOR 6
#define GASD_PRINTTEXT 7
#define GASD_CHARHEIGHT 8
#define GASD_LINE 9
#define GASD_CIRCLE 10
#define GASD_BOX 11
#define GASD_FCIRCLE 12
#define GASD_FLOOD 13

#define GASD_SETWINDOW 14
#define GASD_WIN_LINE 15
#define GASD_WIN_CIRCLE 16
#define GASD_WIN_BOX 17
#define GASD_WIN_FCIRCLE 18
#define GASD_WIN_FLOOD 19
#define GASD_SETVIEWPORT 20

void baseG_ASD(void);
void G_ASDsends(unsigned char *instring);
int chars2int(unsigned char *c1);
double chars2dbl(unsigned char *where);

void swtch2grfxG_ASD(void);
void swtch2textG_ASD(void);
void setfontG_ASD(void);
void colorG_ASD(void);
void printtextG_ASD(void);
void printtest2G_ASD(void);
void lineG_ASD(void);
void circleG_ASD(void);
void boxG_ASD(void);
void fcircleG_ASD(void);
void floodG_ASD(void);
void setwinG_ASD(void);
void wlineG_ASD(void);
void wcircleG_ASD(void);
void wboxG_ASD(void);
void wfcircleG_ASD(void);
void wfloodG_ASD(void);
void setviewG_ASD(void);

void sxToGraphics(int mode);
void sxToText(void);
void sxFontSelect(unsigned char font);
void sxNewLine(void);
void sxClear(void);
void sxColorSelect(int fore, int back);
void sxPrintText(int x, int y, char *text);
void sxDrawLine(int x1, int y1, int x2, int y2);
void sxDrawCircle(int x, int y, int rad);
void sxDrawBox(int x1, int y1, int x2, int y2);
void sxDrawFCircle(int x, int y, int rad);
void sxFloodFill(int x, int y);
void sxSetWindow(int inv, int x1, int y1, int x2, int y2);
void sxwDrawLine(int x1, int y1, int x2, int y2);
void sxwDrawCircle(int x, int y, int r);
void sxwDrawBox(int x1, int y1, int x2, int y2);
void sxwDrawFCircle(int x1, int y1, int r);
void sxwFloodFill(int x, int y);
void sxSetViewPort(int x1, int y1, int x2, int y2);
