/***************************************************************************
 *																									*
 *      ASD Interpreter Code
 **
 *            																							      *
 *      Copyright (c) 1991-1992, Chris Fanning.  All rights reserved. *
 *																									*
 ***************************************************************************/

#include <bios.h>
#include <conio.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "asd.h"
#include "asddat.h"
#include "video.h"

#define KEND 34

int ASD_PORT;

unsigned char ASDRESPOND[] = {0x41, 0x53, 0x44, 0xfe, 0xff, 80, 24, 13, 10};

ASD_COMP ASDcore;

/*********************** KEYBOARD STUFF ************************/
unsigned char key_extend[] = {0x52, 0x47, 0x49, 0x53, 0x4f,
                              0x51, 0x48, 0x4b, 0x50, 0x4d};
unsigned char key_pad[][2] = {'*', 0x37, '-', 0x4a, '+', 0x4e};
/* following is extended keys for keypad #'s... not good!
        ,'7',0x47,'8',0x48,'9',0x49,'4',0x4b,'5',0x4c,'6',0x4d,'1',0x4f,'2',0x50,
        '3',0x51,'0',0x52,'.',0x53}; */
unsigned short key_buff = 0;

void asend(unsigned char inchar) {
  _bios_serialcom(_COM_SEND, ASD_PORT - 1, inchar);
}

unsigned short agetch(void) {
  while (1) {
    if (vKbhit()) return (vGetch());
    if (_bios_serialcom(_COM_STATUS, ASD_PORT - 1, 0) & 0x100)
      return (_bios_serialcom(_COM_RECEIVE, ASD_PORT - 1, 0));
  }
}

short akbhit(void) {
  return ((_bios_serialcom(_COM_STATUS, ASD_PORT - 1, 0) & 0x0100) || vKbhit());
}

void receiveASD(unsigned short (*astdin)(void),
                unsigned short (*stdstat)(void)) {
  ASDcore.recv = astdin;
  ASDcore.rxstat = stdstat;
}

void asends(unsigned char *instring) {
  ASDcore.ASDrepNbuff = 0;
  while (*instring) addcrlf(*instring++);
  comp_flush();
}

void asendc(unsigned char *instring, short count) {
  if (!count) return;
  ASDcore.ASDrepNbuff = 0;
  while (count--) comp_output(*instring++);
  comp_flush();
}

void addcrlf(unsigned char inchar) {
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
  comp_output(inchar);
}

void _cdecl aprintf(unsigned char *constr, ...) {
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
    if (chr == '%') { /* format code */
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

unsigned short vGetch(void) {
  unsigned short scancode = 0, ascii = 0, temp;
  if (key_buff) {
    scancode = key_buff;
    key_buff = 0;
    return scancode;
  }
  _asm xor ax, ax _asm int 16h _asm mov byte ptr scancode,
      ah _asm mov byte ptr ascii, al if (ascii) {
    for (temp = 0; temp < sizeof(key_pad) / 2; temp++)
      if (key_pad[temp][0] == ascii && key_pad[temp][1] == scancode) {
        key_buff = temp + GREYSTAR;
        return 1;
      }
    if (ascii >= 0x20) return ascii;
    switch (scancode) {
      case 0x1c: /* enter */
      case 0xe:  /* backspace */
      case 0xf:  /* tab */
      case 0x1:  /* escape */
        return ascii;
      default:
        key_buff = ascii;
        return 1;
    }
  }
  for (ascii = 0; ascii < sizeof(key_extend); ascii++)
    if (key_extend[ascii] == scancode) {
      key_buff = ascii + INSERT;
      return 1;
    }
  key_buff = scancode;
  return 0;
}

unsigned short vKbhit(void) {
  unsigned short ascii, scancode;
  if (key_buff) return key_buff;
  _asm mov ah, 1 _asm int 16h _asm mov byte ptr ascii,
      al _asm mov byte ptr scancode, ah _asm jnz vKbleave return 0;
vKbleave:
  switch (scancode) {
    case 0x1c: /* enter */
    case 0xe:  /* backspace */
    case 0xf:  /* tab */
    case 0x1:  /* escape */
      return ascii;
  }
  return 1;
}
