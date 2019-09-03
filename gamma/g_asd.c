/* Unofficial extensions to ASD...so what the hell, I didn't ask Chris...*/
/* NOTE:  These puppies currently do not support line noise :)		*/

/*in the end, their transmission system will be over a network, so I
  really don't give a shit what happens*/

/*besides, if ya don't have an error correcting modem, that's your own
  stupid fault*/

#include "g_asd.h"
#include <graph.h>
#include <stdio.h>
#include <string.h>
#include "..\g2\fontop.h"
#include "asd.h"
#include "asddat.h"
#include "video.h"

extern struct _ASD_COMP ASDcore;

union doublenbytes {
  unsigned char uc[8];
  double d;
} t_dnb;

#define ASDSENDARG(x) asend((unsigned char)(255 - x))
#define GASDSENDARG(x)                       \
  {                                          \
    asend((unsigned char)(255 - (x >> 8)));  \
    asend((unsigned char)(255 - (x % 256))); \
  }

void CGA_hidecursor(void) {
  outp(0x3d4, 14);
  outp(0x3d5, 0x1f);
  outp(0x3d4, 15);
  outp(0x3d5, 0x41);
}

void GASDSENDDBL(double x) {
  char *buf;
  int n;
  buf = &x;
  for (n = 0; n < 8; ++n) ASDSENDARG(buf[n]);
}

#define DOUBLESZ sizeof(double)
#define INTSZ sizeof(int)

void baseG_ASD(void) {
  switch (ASDcore.ASDargs[0]) {
    case GASD_SWTCH2GRFX:
      ASDwait(swtch2grfxG_ASD, 2); /* mode (2)*/
      return;
    case GASD_SWTCH2TEXT:
      swtch2textG_ASD();
      return;
    case GASD_SETFONT:
      ASDwait(setfontG_ASD, 1); /*font*/
      return;
    case GASD_NEWLINE:
      fn_newline();
      return;
    case GASD_CLEAR:
      fn_clear();
      return;
    case GASD_COLOR:
      ASDwait(colorG_ASD, 2); /* fore, back*/
      return;
    case GASD_PRINTTEXT:
      ASDwait(printtextG_ASD, 6); /*x loc (2), y loc (2) length of text (2)*/
      return;
    case GASD_CHARHEIGHT:
      ASDwait(printtextG_ASD, 1); /*font number*/
      return;
    case GASD_LINE:
      ASDwait(lineG_ASD, 8); /*x1 (2), y1(2), x2 (2), y2(2)*/
      return;
    case GASD_CIRCLE:
      ASDwait(circleG_ASD, 6); /*x (2), y(2), rad (2)*/
      return;
    case GASD_BOX:
      ASDwait(boxG_ASD, 8); /*see GASD_LINE*/
      return;
    case GASD_FCIRCLE:
      ASDwait(fcircleG_ASD, 6); /*see GASD_CIRCLE*/
      return;
    case GASD_FLOOD:
      ASDwait(floodG_ASD, 4); /*x(2), y(2)*/
      return;
    case GASD_SETWINDOW:
      ASDwait(setwinG_ASD, INTSZ * 5);
      return;
    case GASD_WIN_LINE:
      ASDwait(wlineG_ASD, INTSZ * 4);
      return;
    case GASD_WIN_CIRCLE:
      ASDwait(wcircleG_ASD, INTSZ * 3);
      return;
    case GASD_WIN_BOX:
      ASDwait(wboxG_ASD, INTSZ * 4);
      return;
    case GASD_WIN_FCIRCLE:
      ASDwait(wfcircleG_ASD, INTSZ * 3);
      return;
    case GASD_WIN_FLOOD:
      ASDwait(wfloodG_ASD, INTSZ * 2);
      return;
    case GASD_SETVIEWPORT:
      ASDwait(setviewG_ASD, INTSZ * 4);
      return;
  }
}

void swtch2grfxG_ASD(void) {
  int mode;
  mode = chars2int(ASDcore.ASDargs);
  if (mode != ASDcore.ASDgmode) {
    _setvideomode(mode);
    ASDcore.ASDgmode = mode;
  }
}

void swtch2textG_ASD(void) {
  if (ASDcore.ASDgmode) {
    ASDcore.ASDgmode = 0;
    vInit(FINDADAPTER | CO40);
  }
  CGA_hidecursor();
}

void setfontG_ASD(void) { fn_setfont((int)ASDcore.ASDargs[0]); }

void colorG_ASD(void) {
  int back, fore;

  fore = chars2int(ASDcore.ASDargs);
  back = chars2int(ASDcore.ASDargs + 2);

  fn_color(fore, back);
}

void printtextG_ASD(void) {
  /*x loc (2), y loc (2) length of text (2)*/
  /*here's where we have fun...this is pretty damn un-portable, since it
    takes it upon itself to do the waitforASD stuff itself...please
    remember this when playing*/

  ASDcore.ASDput = ASDgetargs;
  ASDcore.ASDargsleft = chars2int(ASDcore.ASDargs + 4);
  ASDcore.ASDargidx = 6; /*note: we want to leave the first four args
                           on the argument stack...trust me*/
  ASDcore.ASDcurfn = printtest2G_ASD;
  ASDcore.ASDsafe = NOTSAFE;
}

void printtest2G_ASD(void) {
  /*hmm, args 0&1 are xpos, 2&3 are ypos & 4- is a null term string*/

  /*gotta null term the string, dude*/

  ASDcore.ASDargs[6 + chars2int(ASDcore.ASDargs + 4)] = 0;

  fn_printtext(chars2int(ASDcore.ASDargs + 0), chars2int(ASDcore.ASDargs + 2),
               &ASDcore.ASDargs[6]);
}

void lineG_ASD(void) {
  /*x1 (2), y1(2), x2 (2), y2(2)*/

  _moveto(chars2int(ASDcore.ASDargs), chars2int(ASDcore.ASDargs + 2));
  _lineto(chars2int(ASDcore.ASDargs + 4), chars2int(ASDcore.ASDargs + 6));
}

void circleG_ASD(void) {
  int x1, y1, rad;

  x1 = chars2int(ASDcore.ASDargs + 0);
  y1 = chars2int(ASDcore.ASDargs + 2);
  rad = chars2int(ASDcore.ASDargs + 4);
  _ellipse(_GBORDER, x1 - rad, y1 - rad, x1 + rad, y1 + rad);
}

void boxG_ASD(void) {
  _rectangle(_GBORDER, chars2int(ASDcore.ASDargs),
             chars2int(ASDcore.ASDargs + 2), chars2int(ASDcore.ASDargs + 4),
             chars2int(ASDcore.ASDargs + 6));
}

void fcircleG_ASD(void) {
  int x1, y1, rad;

  x1 = chars2int(ASDcore.ASDargs);
  y1 = chars2int(ASDcore.ASDargs + 2);
  rad = chars2int(ASDcore.ASDargs + 4);
  _ellipse(_GFILLINTERIOR, x1 - rad, y1 - rad, x1 + rad, y1 + rad);
}

void floodG_ASD(void) {
  _floodfill(chars2int(ASDcore.ASDargs), chars2int(ASDcore.ASDargs + 2),
             _getcolor());
}

void setwinG_ASD(void) {
  _setwindow(chars2int(ASDcore.ASDargs), (double)chars2int(ASDcore.ASDargs + 2),
             (double)chars2int(ASDcore.ASDargs + 4),
             (double)chars2int(ASDcore.ASDargs + 6),
             (double)chars2int(ASDcore.ASDargs + 8));
}

void wlineG_ASD(void) {
  _moveto_w((double)chars2int(ASDcore.ASDargs),
            (double)chars2int(ASDcore.ASDargs + 2));
  _lineto_w((double)chars2int(ASDcore.ASDargs + 4),
            (double)chars2int(ASDcore.ASDargs + 6));
}

void wcircleG_ASD(void) {
  int x, y, r;
  x = chars2int(ASDcore.ASDargs + 0);
  y = chars2int(ASDcore.ASDargs + 2);
  r = chars2int(ASDcore.ASDargs + 4);

  _ellipse_w(_GBORDER, (double)(x - r), (double)(y - r), (double)(x + r),
             (double)(y + r));
}

void wboxG_ASD(void) {
  _rectangle_w(_GBORDER, (double)chars2int(ASDcore.ASDargs + 0),
               (double)chars2int(ASDcore.ASDargs + 2),
               (double)chars2int(ASDcore.ASDargs + 4),
               (double)chars2int(ASDcore.ASDargs + 6));
}

void wfcircleG_ASD(void) {
  int x, y, r;
  x = chars2int(ASDcore.ASDargs + 0);
  y = chars2int(ASDcore.ASDargs + 2);
  r = chars2int(ASDcore.ASDargs + 4);

  _ellipse_w(_GFILLINTERIOR, (double)(x - r), (double)(y - r), (double)(x + r),
             (double)(y + r));
}

void wfloodG_ASD(void) {
  _floodfill_w((double)chars2int(ASDcore.ASDargs + 0),
               (double)chars2int(ASDcore.ASDargs + 2), _getcolor());
}

void setviewG_ASD(void) {
  _setviewport(chars2int(ASDcore.ASDargs + 0), chars2int(ASDcore.ASDargs + 2),
               chars2int(ASDcore.ASDargs + 4), chars2int(ASDcore.ASDargs + 6));
  _setcliprgn(chars2int(ASDcore.ASDargs + 0), chars2int(ASDcore.ASDargs + 2),
              chars2int(ASDcore.ASDargs + 4), chars2int(ASDcore.ASDargs + 6));
}

void sxToGraphics(int mode) {
  asend(GASDS);
  asend((unsigned char)255 - GASD_SWTCH2GRFX);

  GASDSENDARG(mode);
}

void sxToText(void) {
  asend(GASDS);
  asend((unsigned char)255 - GASD_SWTCH2TEXT);
}

void sxFontSelect(unsigned char font) {
  asend(GASDS);
  asend((unsigned char)255 - GASD_SETFONT);
  asend((unsigned char)(255 - font));
}

void sxNewLine(void) {
  asend(GASDS);
  asend((unsigned char)255 - GASD_NEWLINE);
}

void sxClear(void) {
  asend(GASDS);
  asend((unsigned char)255 - GASD_CLEAR);
}

void sxColorSelect(int fore, int back) {
  asend(GASDS);
  asend((unsigned char)255 - GASD_COLOR);
  GASDSENDARG(fore);
  GASDSENDARG(back);
}

void sxPrintText(int x, int y, char *text) {
  int a;
  asend(GASDS);
  ASDSENDARG(GASD_PRINTTEXT);
  GASDSENDARG(x);
  GASDSENDARG(y);
  a = strlen(text);
  GASDSENDARG(a);
  G_ASDsends(text);
}

void sxDrawLine(int x1, int y1, int x2, int y2) {
  asend(GASDS);
  ASDSENDARG(GASD_LINE);
  GASDSENDARG(x1);
  GASDSENDARG(y1);
  GASDSENDARG(x2);
  GASDSENDARG(y2);
}

void sxDrawCircle(int x, int y, int rad) {
  asend(GASDS);
  ASDSENDARG(GASD_CIRCLE);
  GASDSENDARG(x);
  GASDSENDARG(y);
  GASDSENDARG(rad);
}

void sxDrawBox(int x1, int y1, int x2, int y2) {
  asend(GASDS);
  ASDSENDARG(GASD_BOX);
  GASDSENDARG(x1);
  GASDSENDARG(y1);
  GASDSENDARG(x2);
  GASDSENDARG(y2);
}

void sxDrawFCircle(int x, int y, int rad) {
  asend(GASDS);
  ASDSENDARG(GASD_FCIRCLE);
  GASDSENDARG(x);
  GASDSENDARG(y);
  GASDSENDARG(rad);
}

void sxFloodFill(int x, int y) {
  asend(GASDS);
  ASDSENDARG(GASD_FLOOD);
  GASDSENDARG(x);
  GASDSENDARG(y);
}

void sxSetWindow(int inv, int x1, int y1, int x2, int y2) {
  asend(GASDS);
  ASDSENDARG(GASD_SETWINDOW);
  GASDSENDARG(inv);
  GASDSENDARG(x1);
  GASDSENDARG(y1);
  GASDSENDARG(x2);
  GASDSENDARG(y2);
}

void sxwDrawLine(int x1, int y1, int x2, int y2) {
  asend(GASDS);
  ASDSENDARG(GASD_WIN_LINE);
  GASDSENDARG(x1);
  GASDSENDARG(y1);
  GASDSENDARG(x2);
  GASDSENDARG(y2);
}

void sxwDrawCircle(int x, int y, int r) {
  asend(GASDS);
  ASDSENDARG(GASD_WIN_CIRCLE);
  GASDSENDARG(x);
  GASDSENDARG(y);
  GASDSENDARG(r);
}

void sxwDrawBox(int x1, int y1, int x2, int y2) {
  asend(GASDS);
  ASDSENDARG(GASD_WIN_BOX);
  GASDSENDARG(x1);
  GASDSENDARG(y1);
  GASDSENDARG(x2);
  GASDSENDARG(y2);
}

void sxwDrawFCircle(int x, int y, int r) {
  asend(GASDS);
  ASDSENDARG(GASD_WIN_FCIRCLE);
  GASDSENDARG(x);
  GASDSENDARG(y);
  GASDSENDARG(r);
}

void sxwFloodFill(int x, int y) {
  asend(GASDS);
  ASDSENDARG(GASD_WIN_FLOOD);
  GASDSENDARG(x);
  GASDSENDARG(y);
}

void sxSetViewPort(int x1, int y1, int x2, int y2) {
  asend(GASDS);
  ASDSENDARG(GASD_SETVIEWPORT);
  GASDSENDARG(x1);
  GASDSENDARG(y1);
  GASDSENDARG(x2);
  GASDSENDARG(y2);
}

int chars2int(unsigned char *where) {
  return (int)((where[0] * 256) + where[1]);
}

double chars2dbl(unsigned char *where) {
  double *temp;
  temp = (double *)where;
  return (*temp);
}

void G_ASDsends(unsigned char *instring) {
  ASDcore.ASDrepNbuff = 0;
  while (*instring || ASDcore.ASDsafe != SAFE)
    asend((unsigned char)(255 - (*instring++)));
}
