/***************************************************************************
 *																									*
 *	  ASD Interpreter Code
 **
 *																									*
 *	  Copyright (c) 1991-1992, Chris Fanning.  All rights reserved.
 **
 *																									*
 ***************************************************************************/

#include "asd.h"
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include "asddat.h"
#include "g_asd.h"
#include "video.h"

unsigned char ASDRESPOND[] = {0x41, 0x53, 0x44, 0xfe, 0xff, 80, 24, 13, 10};

ASD_COMP ASDcore;

int ASDInit(char adapter) {
  memset(&ASDcore, 0, sizeof(ASD_COMP));
  ASDcore.output = vPutch;
  ASDcore.rxstat = vKbhit;
  ASDcore.recv = vGetch;
  ASDcore.ASDput = baseASD;
  ASDcore.ASDsafe = SAFE;
  ASDcore.ASDgmode = 0;
  return vInit(adapter);
}

short ASDDeInit(void) {
  if (ASDcore.SavedState) free(ASDcore.SavedState);
  return vDeInit();
}

void asend(unsigned char inchar) {
  if (ASDcore.rawout) ASDcore.rawout(inchar);
  if (ASDcore.output) ASDcore.ASDput(inchar);
}

unsigned short agetch(void) { return ASDcore.recv(); }

short akbhit(void) { return ASDcore.rxstat(); }

void receiveASD(unsigned short (*astdin)(void),
                unsigned short (*stdstat)(void)) {
  ASDcore.recv = astdin;
  ASDcore.rxstat = stdstat;
}

void getreceiveASD(unsigned short (**astdin)(void),
                   unsigned short (**stdstat)(void)) {
  *astdin = ASDcore.recv;
  *stdstat = ASDcore.rxstat;
}

void asends(unsigned char *instring) {
  ASDcore.ASDrepNbuff = 0;
  while (*instring || ASDcore.ASDsafe != SAFE) addcrlf(*instring++);
  comp_flush();
}

void asendc(unsigned char *instring, short count) {
  if (!count) return;
  ASDcore.ASDrepNbuff = 0;
  while (count--) comp_output(*instring++);
  comp_flush();
}

void addcrlf(unsigned char inchar) {
  if (ASDcore.ASDsafe == SAFE) {
    if (inchar == 13) {
      comp_output(inchar);
      comp_output(10);
      return;
    }
    if (inchar == 10) {
      comp_output(13);
      comp_output(inchar);
      return;
    }
  }
  comp_output(inchar);
}

void baseASD(unsigned char inchar) {
  short lp;
  switch (inchar) {
    case ANSWR:
      if (ASDcore.respnd) {
        for (lp = 0; lp < sizeof(ASDRESPOND); lp++)
          ASDcore.respnd(ASDRESPOND[lp]);
      }
      return;
    case EXTND:
      ASDwait(extndASD, 1);
      return;
    case ESCAP:
      ASDwait(escapASD, 1);
      return;
    case BACKS:
      vBACKS();
      return;
    case LF:
      vLF();
      return;
    case CHATR:
      ASDwait(chatrASD, 1);
      return;
    case FF:
      vClearScreen();
      return;
    case CR:
      vCR();
      return;
    case POSCR:
      ASDwait(poscrASD, 2);
      return;
    case BOX:
      ASDwait(boxASD, 5);
      return;
    case CURSR:
      ASDwait(cursrASD, 1);
      return;
    case VERTR:
      ASDwait(vertrASD, 1);
      return;
    case HORIR:
      ASDwait(horirASD, 1);
      return;
    case HORRP:
      ASDwait(horrpASD, 2);
      return;
    case VERRP:
      ASDwait(verrpASD, 2);
      return;
    case IMAGE:
      ASDcore.ASDput = imageASD;
      return;
    case INSDL:
      ASDwait(insdlASD, 1);
      return;
    case LSCRL:
      ASDwait(lscrlASD, 1);
      return;
    case BORDR:
      ASDwait(bordrASD, 3);
      return;
    case ATTRC:
      ASDwait(attrcASD, 2);
      return;
    case REPET:
      ASDwait(repetASD, 2);
      return;
    case PAGES:
      ASDwait(pagesASD, 1);
      return;
      /*DEF'N added for G_ASD codes*/
#ifdef EXTENDED_GRAPHICS
    case GASDS:
      ASDwait(baseG_ASD, 1);
      return;
#endif
  }
  if (ASDcore.output) ASDcore.output(inchar);
}

void ASDwait(void (*fn_action)(), unsigned char waitfor) {
  ASDcore.ASDput = ASDgetargs;
  ASDcore.ASDargsleft = waitfor;
  ASDcore.ASDargidx = 0;
  ASDcore.ASDcurfn = fn_action;
  ASDcore.ASDsafe = NOTSAFE;
}

void ASDgetargs(unsigned char inchar) {
  ASDcore.ASDargs[ASDcore.ASDargidx++] = (unsigned char)(255 - inchar);
  if (--ASDcore.ASDargsleft) return;
  ASDcore.ASDsafe = SAFE;
  ASDcore.ASDput = baseASD;
  ASDcore.ASDcurfn();
}

void respondASD(void (*newres)(unsigned char)) { ASDcore.respnd = newres; }

void escapASD(void) { /* print ANY character */
  if (ASDcore.output) ASDcore.output(ASDcore.ASDargs[0]);
}

void chatrASD(void) { /* change attribute */
  vChangeAttr(ASDcore.ASDargs[0]);
}

void extndASD(void) { /* do nada I guess? */
}

void poscrASD(void) { /* move cursor to X,Y */
  vPosCur(ASDcore.ASDargs[0], ASDcore.ASDargs[1]);
}

void boxASD(void) { /* draw a box on the screen */
  vBox((ASDcore.ASDargs[0] * 256) + ASDcore.ASDargs[1], ASDcore.ASDargs[2],
       ASDcore.ASDargs[3], ASDcore.ASDargs[4]);
}

void cursrASD(void) { /* change something with the cursor */
  vCursorControl(ASDcore.ASDargs[0]);
}

void vertrASD(void) { vVertRel(ASDcore.ASDargs[0]); }

void horirASD(void) { vHorizRel(ASDcore.ASDargs[0]); }

void horrpASD(void) { vRepeatHVC(ASDcore.ASDargs[0], ASDcore.ASDargs[1]); }

void imageASD(unsigned char inchar) {
  switch ((unsigned char)inchar) {
    case 255:
      ASDwait(getimageASD, 3);
      return;
    case 254:
      ASDwait(putimageASD, 1);
      return;
    case 253:
      ASDwait(resetoneASD, 1);
      return;
    case 252:
      vResetImage();
      break;
  }
  ASDcore.ASDput = baseASD;
}

void getimageASD(void) {
  vGetImage(ASDcore.ASDargs[0], ASDcore.ASDargs[1], ASDcore.ASDargs[2]);
}

void putimageASD(void) { vPutImage(ASDcore.ASDargs[0]); }

void insdlASD(void) { vLineEdit(ASDcore.ASDargs[0]); }

void lscrlASD(void) { vLineScroll(ASDcore.ASDargs[0]); }

void resetoneASD(void) { vResetOne(ASDcore.ASDargs[0]); }

void bordrASD(void) {
  vBorder(ASDcore.ASDargs[0], ASDcore.ASDargs[1], ASDcore.ASDargs[2]);
}

void verrpASD(void) { vRepeatVVC(ASDcore.ASDargs[0], ASDcore.ASDargs[1]); }

void attrcASD(void) { vAttrChange(ASDcore.ASDargs[0], ASDcore.ASDargs[1]); }

void repetASD(void) { vRepeat(ASDcore.ASDargs[0], ASDcore.ASDargs[1]); }

void pagesASD(void) { vPage(ASDcore.ASDargs[0]); }

void cdecl aprintf(unsigned char *constr, ...) {
  va_list vargs;
  va_start(vargs, constr);
  ASDcore.ASDrepNbuff = 0;
  _format(addcrlf, constr, vargs);
  comp_flush();
}

void comp_output(unsigned char inchar) {
  if (!ASDcore.ASDrepNbuff) {
    ASDcore.ASDrepNbuff++;
    ASDcore.ASDrepCbuff = inchar;
    return;
  }
  if (inchar == ASDcore.ASDrepCbuff && ASDcore.ASDrepNbuff < 255) {
    ASDcore.ASDrepNbuff++;
    return;
  }
  comp_flush();
  ASDcore.ASDrepCbuff = inchar;
  ASDcore.ASDrepNbuff = 1;
}

void comp_flush(void) {
  sRepeatChar(ASDcore.ASDrepCbuff, ASDcore.ASDrepNbuff);
  ASDcore.ASDrepNbuff = 0;
}

void _format(void (*outptr)(unsigned char), unsigned char *inptr, char *vargs) {
  unsigned char out_buff[17], *ptr, justify, zero, minus, chr;
  unsigned long value, i, width;
  while (chr = *inptr++) {
    if (chr == '%' && ASDcore.ASDsafe == SAFE) /* otherwise it's in a seq */
    {                                          /* format code */
      chr = *inptr++;
      *(ptr = &out_buff[16]) = justify = minus = width = value = i = 0;
      zero = ' ';
      if (chr == '-') { /* left justify */
        --justify;
        chr = *inptr++;
      }
      if (chr == '0') /* leading zeros */
        zero = '0';
      while (chr >= '0' && chr <= '9') { /* field width specifier */
        width = (width * 10) + (chr - '0');
        chr = *inptr++;
      }
      switch (chr) {
        case 'd': /* decimal number */
          value = va_arg(vargs, int);
          if (value & 0x8000) {
            ++minus;
            value = -value;
          }
          i = 10;
          break;
        case 'u': /* unsigned number */
          value = va_arg(vargs, short);
          i = 10;
          break;
        case 'x': /* hexidecimal number */
          i = 16;
          value = va_arg(vargs, short);
          break;
        case 'o': /* octal number */
          i = 8;
          value = va_arg(vargs, short);
          break;
        case 'b': /* binary number */
          i = 2;
          value = va_arg(vargs, short);
          break;
        case 'l': /* long */
          value = va_arg(vargs, long);
          i = 10;
          break;
        case 'c': /* character data */
          *--ptr = (unsigned char)va_arg(vargs, int);
          break;
        case 's': /* string */
          ptr = va_arg(vargs, unsigned char *);
          break;
        default: /* all others */
          *--ptr = chr;
          ++inptr;
      }
      if (i) /* for all numbers, generate the ASCII string */
        do {
          if ((chr = (unsigned char)(value % i) + '0') > '9') chr += 7;
          *--ptr = chr;
        } while (value /= i);
      if (minus) /* output sign if any */
      {
        outptr('-');
        if (width) --width;
      }
      if (width &&
          !justify) { /* pad with 'zero' value if right justify enabled  */
        i = 0;
        while (ptr[i]) i++;
        for (; i < width; ++i) outptr(zero);
      }
      i = 0; /* move in data */
      value = width - 1;
      while ((*ptr) && (i <= value)) {
        outptr(*ptr++);
        ++i;
      }
      if (width &&
          justify) { /* pad with 'zero' value if left justify enabled */
        while (i < width) {
          outptr(zero);
          ++i;
        }
      }
    } else
      outptr(chr); /* not a format code, simply display the character */
  }
}

short newstdoutASD(void (*filtered)(unsigned char),
                   void (*unfiltered)(unsigned char)) {
  ASDcore.rawout = unfiltered;
  ASDcore.output = filtered;
  return (SAFE);
}

void getstdoutASD(void (**filtered)(unsigned char),
                  void (**unfiltered)(unsigned char)) {
  *filtered = ASDcore.output;
  *unfiltered = ASDcore.rawout;
}

void resetASD(void) {
  memset(&ASDcore, 0, sizeof(ASD_COMP));
  ASDcore.output = vPutch;
  ASDcore.rxstat = vKbhit;
  ASDcore.recv = vGetch;
  ASDcore.ASDput = baseASD;
  ASDcore.ASDsafe = SAFE;
  vReset();
}

void sBox(unsigned short mask, unsigned char width, unsigned char height,
          unsigned char filler) {
  asend(BOX);
  asend((unsigned char)(255 - (mask >> 8)));
  asend((unsigned char)(255 - (mask % 256)));
  asend((unsigned char)(255 - filler));
  asend((unsigned char)(255 - width));
  asend((unsigned char)(255 - height));
}

void sGetImage(unsigned char width, unsigned char height, unsigned char buffr) {
  asend(IMAGE);
  asend(255);
  asend((unsigned char)(255 - width));
  asend((unsigned char)(255 - height));
  asend((unsigned char)(255 - buffr));
}

void sPutImage(unsigned char buffr) {
  asend(IMAGE);
  asend(254);
  asend((unsigned char)(255 - buffr));
}

void sEscape(unsigned char escchar) {
  asend(ESCAP);
  asend((unsigned char)(255 - escchar));
}

void sChangeAttr(unsigned char newattr) {
  asend(CHATR);
  asend((unsigned char)(255 - newattr));
}

void sPositionCur(unsigned char newx, unsigned char newy) {
  asend(POSCR);
  asend((unsigned char)(255 - newx));
  asend((unsigned char)(255 - newy));
}

void sCurShape(unsigned char shape) {
  if (shape < CURSOR_SIZES) {
    asend(CURSR);
    asend((unsigned char)(255 - shape));
  }
}

void sCurControl(unsigned char action) {
  if ((action >= CUR_SAVE) && (action <= CUR_UPDATE)) {
    asend(CURSR);
    asend((unsigned char)(255 - action));
  }
}

void sCurUp(unsigned char count) {
  asend(VERTR);
  asend((unsigned char)(255 - (count | 0x80)));
}

void sCurDown(unsigned char count) {
  asend(VERTR);
  asend((unsigned char)(255 - (count & 0x7f)));
}

void sCurBack(unsigned char count) {
  asend(HORIR);
  asend((unsigned char)(255 - (count | 0x80)));
}

void sCurForward(unsigned char count) {
  asend(HORIR);
  asend((unsigned char)(255 - (count & 0x7f)));
}

void sBorder(unsigned char width, unsigned char height, unsigned char style) {
  if (style < BORDER_TYPES) {
    asend(BORDR);
    asend((unsigned char)(255 - width));
    asend((unsigned char)(255 - height));
    asend((unsigned char)(255 - style));
  }
}

void sChangeBar(unsigned char color, unsigned char run) {
  asend(ATTRC);
  asend((unsigned char)(255 - color));
  asend((unsigned char)(255 - run));
}

void sScrollLeft(unsigned char num) {
  asend(LSCRL);
  asend((unsigned char)(255 - (num | 0x80)));
}

void sScrollRight(unsigned char num) {
  asend(LSCRL);
  asend((unsigned char)(255 - (num & 0x7f)));
}

void sFreeHandle(unsigned char handle) {
  if (!handle) return;
  asend(IMAGE);
  asend(253);
  asend((unsigned char)(255 - handle));
}

void sRepeatChar(unsigned char rchar, unsigned char nrep) {
  unsigned char lp;
  if (nrep < 4) {
    for (lp = 0; lp < nrep; lp++) asend(rchar);
    return;
  }
  asend(REPET);
  asend((unsigned char)(255 - rchar));
  asend((unsigned char)(255 - nrep));
}

void sLineEdit(unsigned char action) {
  asend(INSDL);
  asend((unsigned char)(255 - action));
}

void sPageSelect(unsigned char pagenum) {
  if (pagenum < NUMPAGES) {
    asend(PAGES);
    asend((unsigned char)(255 - pagenum));
  }
}

void sPageCopy(unsigned char to_page) {
  if (to_page < 2) /* two ASD functions, to_visual & to_current */
  {                /* can be CUR_TO_VISUAL or CUR_FROM_VISUAL */
    asend(PAGES);
    asend((unsigned char)(255 - (to_page + NUMPAGES)));
  }
}

void sVerLine(unsigned char vector, unsigned char inchar) {
  asend(VERRP);
  asend((unsigned char)(255 - vector));
  asend((unsigned char)(255 - inchar));
}

void sImageReset(void) {
  asend(IMAGE);
  asend(252);
}
