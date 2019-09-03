/***************************************************************************
 *																									*
 *	  Text Video Routines
 **
 *																									*
 *	  Copyright (c) 1991 Chris Fanning.  All rights reserved.
 ** This file is not for public consumption!
 **
 *																									*
 ***************************************************************************/

#include "video.h"
#include <conio.h>
#include <dos.h>
#include <malloc.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "videodat.h"

struct vcontrol vbase;
vpagestuff apages[NUMPAGES];

unsigned char cur_sizes[CURSOR_SIZES][ADAPTER_TYPES][2] = {
    32, 0,  32, 0,  32, 0,
    4,  2,  32, 0, /* Hidden Cursor */
    0,  2,  0,  1,  0,  3,
    0,  1,  0,  2, /* Overbar */
    0,  6,  0,  3,  0,  6,
    0,  3,  0,  6, /* Half-Overbar */
    0,  12, 0,  7,  0,  13,
    0,  7,  0,  14, /* Full Cursor */
    6,  12, 4,  7,  6,  13,
    4,  7,  6,  14, /* Half-Underbar */
    11, 12, 6,  7,  11, 13,
    6,  7,  13, 14 /* Underbar */
    /* MDA	CGA	EGA	MCGA	VGA	*/
};
unsigned char borders[BORDER_TYPES][8] = {
    'Ú', '¿', 'À', 'Ù', 'Ä', 'Ä', '³', '³', /* 0 */
    'Ö', '·', 'Ó', '½', 'Ä', 'Ä', 'º', 'º', /* 1 */
    'Õ', '¸', 'Ô', '¾', 'Í', 'Í', '³', '³', /* 2 */
    'É', '»', 'È', '¼', 'Í', 'Í', 'º', 'º', /* 3 */
    'Ö', '¿', 'Ó', 'Ù', 'Ä', 'Ä', 'º', '³', /* 4 */
    'Ú', '·', 'Ô', '¼', 'Ä', 'Í', '³', 'º', /* 5 */
    'É', '¸', 'Ó', 'Ù', 'Í', 'Ä', 'º', '³', /* 6 */
    'Û', 'Û', 'Û', 'Û', 'ß', 'Ü', 'Û', 'Û', /* 7 */
    'Û', 'Û', 'Û', 'Û', 'Û', 'Û', 'Û', 'Û', /* 8 */
    'Û', 'Û', 'Û', 'Û', 'Û', 'Û', 'Ý', 'Þ', /* 9 */
    'Û', 'Û', 'Û', 'Û', 'ß', 'Ü', 'Ý', 'Þ'  /* 10 */
};

/*********************** KEYBOARD STUFF ************************/
unsigned char key_extend[] = {0x52, 0x47, 0x49, 0x53, 0x4f,
                              0x51, 0x48, 0x4b, 0x50, 0x4d};
unsigned char key_pad[][2] = {'*', 0x37, '-', 0x4a, '+', 0x4e};
/* following is extended keys for keypad #'s... not good!
        ,'7',0x47,'8',0x48,'9',0x49,'4',0x4b,'5',0x4c,'6',0x4d,'1',0x4f,'2',0x50,
        '3',0x51,'0',0x52,'.',0x53}; */
unsigned short key_buff = 0;

int vInit(char adapter) {
  struct VIDhandle VIDstruct;
  unsigned short numbytes = IM_MEMTOTAL;
  memset(&vbase, 0, sizeof(vbase));
  memset(apages, 0, sizeof(apages));
  vbase.imemleft = IM_MEMTOTAL / 2;
  if (adapter & CO40) {
    apages[0].swidth = 40;
    apages[0].textmode = 0;
  } else {
    apages[0].swidth = 80;
    apages[0].textmode = 3;
  }

  if ((adapter & ~CO40) == FINDADAPTER) {
    VideoID(&VIDstruct);
    adapter = VIDstruct.Video0Type;
  }
  if (VIDstruct.Video0Type == MDA || VIDstruct.Video0Type & 0x80 ||
      !VIDstruct.Video0Type)
    apages[0].cardref = 0;
  else
    apages[0].cardref = adapter - 1;
  if (VIDstruct.Video1Type == MDA || VIDstruct.Video1Type & 0x80 ||
      !VIDstruct.Video1Type)
    apages[1].cardref = 0;
  else
    apages[1].cardref = adapter - 1;

  apages[0].crsupdat = apages[1].crsupdat = YES_UPDATE;
  apages[0].curattr = apages[1].curattr = DEFAULT_ATTR;
  apages[0].pagemem = (unsigned short _far *)0xb8000000;
  apages[0].height = 24;
  apages[0].CRTC = 0x3d4;
  apages[0].numidx = apages[0].swidth * apages[0].height;
  vmode(apages[0].textmode);
  apages[1].swidth = 80;
  apages[1].height = 24;
  apages[1].pagemem = (unsigned short _far *)0xb0000000;
  apages[1].CRTC = 0x3b4;
  apages[1].numidx = apages[1].swidth * apages[1].height;
  apages[1].textmode = 7;
  /*vmode(apages[1].textmode);*/
  if (!apages[0].cardref)
    vbase.pages = apages[1];
  else
    vbase.pages = apages[0];
  numbytes += vbase.pages.swidth * 2; /* enough for a line of scrap */
  numbytes += apages[0].numidx * 2;   /* another screenfull */
  if (!(vbase.imagemem = (unsigned short _far *)_fmalloc(numbytes)))
    return (VNOMEM);
  _fmemset(vbase.imagemem, 0, numbytes);

  vbase.linemem = &vbase.imagemem[IM_MEMTOTAL / 2 + vbase.pages.numidx];
  vbase.curpage = 0;

  vCursorControl(DEFAULT_CURSOR);
  vBlinkControl(BLOFF);
  return (VOK);
}

int vDeInit(void) {
  _ffree(vbase.imagemem);
  vCursorControl(CUR_UNDER);
  vBlinkControl(BLOFF);
  vmode(vbase.pages.textmode);
  return (0);
}

void vBlinkControl(char onoff) {
  /*	unsigned short portno, blink;
          blink = onoff == BLON ? 1 : 0;
          if (vbase.pages.cardref < 2)
                  {
                  if (vbase.pages.cardref == 0)
                          portno = 0x3b8;
                  else portno = 0x3d8;
                  outp(portno, 9|(blink<<5));
                  return;
                  }
          else
                  {/*
                  /*_asm	mov	bx, blink				/* ax =
     onoff *//*
		_asm	mov	ax, 0x1003
		_asm	int	0x10
		}*/
}

void vScrollUp(void) {
  unsigned short lp;
  lp = vbase.pages.numidx - vbase.pages.swidth;
  _fmemmove(vbase.pages.pagemem, &vbase.pages.pagemem[vbase.pages.swidth],
            (vbase.pages.numidx - vbase.pages.swidth) * 2);
  for (; lp < vbase.pages.numidx; lp++)
    vbase.pages.pagemem[lp] = vbase.pages.curattr << 8;
}

void vCursorControl(unsigned char curact) {
  if (curact < CURSOR_SIZES) {
    vCurType(curact);
    return;
  }
  if (curact == CUR_SAVE) {
    if (vbase.pages.storedidx < CNSAV) {
      vbase.pages.stored[vbase.pages.storedidx].ctype = vbase.pages.curtype;
      vbase.pages.stored[vbase.pages.storedidx].crsx =
          vbase.pages.writepos.crsx;
      vbase.pages.stored[vbase.pages.storedidx].crsy =
          vbase.pages.writepos.crsy;
      vbase.pages.stored[vbase.pages.storedidx].idx = vbase.pages.writepos.idx;
      vbase.pages.storedidx++;
    }
    return;
  }
  if (curact == CUR_RESTORE) {
    if (vbase.pages.storedidx) {
      vbase.pages.storedidx--;
      vbase.pages.writepos.crsx =
          vbase.pages.stored[vbase.pages.storedidx].crsx;
      vbase.pages.writepos.crsy =
          vbase.pages.stored[vbase.pages.storedidx].crsy;
      vbase.pages.writepos.idx = vbase.pages.stored[vbase.pages.storedidx].idx;
      vbase.pages.curtype = vbase.pages.stored[vbase.pages.storedidx].ctype;
      vCurType(vbase.pages.curtype);
      vCupDat();
    }
    return;
  }
  if (curact == CUR_NOUPDATE && vbase.pages.crsupdat == YES_UPDATE) {
    vbase.pages.crspos.crsx = vbase.pages.writepos.crsx;
    vbase.pages.crspos.crsy = vbase.pages.writepos.crsy;
    vbase.pages.crspos.idx = vbase.pages.writepos.idx;
    vbase.pages.crsupdat = NO_UPDATE;
    return;
  }
  if (curact == CUR_UPDATE && vbase.pages.crsupdat == NO_UPDATE) {
    vbase.pages.writepos.crsx = vbase.pages.crspos.crsx;
    vbase.pages.writepos.crsy = vbase.pages.crspos.crsy;
    vbase.pages.writepos.idx = vbase.pages.crspos.idx;
    vbase.pages.crsupdat = YES_UPDATE;
  }
}

void vClearScreen(void) {
  unsigned short vidx = vbase.pages.numidx;
  while (vidx--) vbase.pages.pagemem[vidx] = vbase.pages.curattr << 8;
  vbase.pages.writepos.crsx = 0;
  vbase.pages.writepos.crsy = 0;
  vbase.pages.writepos.idx = 0;
  vCupDat();
}

void vChangeAttr(unsigned char nattr) { vbase.pages.curattr = nattr; }

unsigned char vGetAttr(void) { return vbase.pages.curattr; }

void vPosCur(unsigned char nx, unsigned char ny) {
  if (nx >= vbase.pages.swidth) nx = vbase.pages.swidth - 1;
  if (ny >= vbase.pages.height) ny = vbase.pages.height - 1;
  vbase.pages.writepos.crsx = nx;
  vbase.pages.writepos.crsy = ny;
  vbase.pages.writepos.idx = (ny * vbase.pages.swidth) + nx;
  vCupDat();
}

void vCR(void) {
  vbase.pages.writepos.idx -= vbase.pages.writepos.crsx;
  vbase.pages.writepos.crsx = 0;
  vCupDat();
}

void vLF(void) {
  if (++vbase.pages.writepos.crsy >= vbase.pages.height) {
    vScrollUp();
    vbase.pages.writepos.crsy--;
  } else {
    vbase.pages.writepos.idx += vbase.pages.swidth;
    vCupDat();
  }
}

void vBACKS(void) {
  if (vbase.pages.writepos.idx) {
    vbase.pages.writepos.idx--;
    if (vbase.pages.writepos.crsx)
      vbase.pages.writepos.crsx--;
    else {
      vbase.pages.writepos.crsx = (unsigned char)(vbase.pages.swidth - 1);
      vbase.pages.writepos.crsy--;
    }
  }
  vCupDat();
}

void vHorizRel(unsigned char vector) {
  if (vector & HIGHBIT) {
    vector &= ~HIGHBIT;
    if (vbase.pages.writepos.crsx > vector) {
      vbase.pages.writepos.crsx -= vector;
      vbase.pages.writepos.idx -= vector;
    } else {
      vbase.pages.writepos.idx -= vbase.pages.writepos.crsx;
      vbase.pages.writepos.crsx = 0;
    }
  } else {
    if (vbase.pages.writepos.crsx + vector < vbase.pages.swidth) {
      vbase.pages.writepos.crsx += vector;
      vbase.pages.writepos.idx += vector;
    } else {
      vbase.pages.writepos.idx +=
          vbase.pages.swidth - vbase.pages.writepos.crsx - 1;
      vbase.pages.writepos.crsx = (unsigned char)(vbase.pages.swidth - 1);
    }
  }
  vCupDat();
}

void vVertRel(unsigned char vector) {
  if (vector & HIGHBIT) {
    vector &= ~HIGHBIT;
    if (vbase.pages.writepos.crsy > vector) {
      vbase.pages.writepos.crsy -= vector;
      vbase.pages.writepos.idx -= vector * vbase.pages.swidth;
    } else {
      vbase.pages.writepos.idx = vbase.pages.writepos.crsx;
      vbase.pages.writepos.crsy = 0;
    }
  } else {
    if (vbase.pages.writepos.crsy + vector < vbase.pages.height) {
      vbase.pages.writepos.crsy += vector;
      vbase.pages.writepos.idx += vector * vbase.pages.swidth;
    } else {
      vbase.pages.writepos.idx =
          vbase.pages.numidx - (vbase.pages.swidth - vbase.pages.writepos.crsx);
      vbase.pages.writepos.crsy = (unsigned char)(vbase.pages.height - 1);
    }
  }
  vCupDat();
}

void vRepeatHVC(unsigned char rchar, unsigned char vector) {
  unsigned short cxidx;
  unsigned char xgoto, cursoradjust = 0;
  if (vector & HIGHBIT) {
    vector &= ~HIGHBIT;
    if (vbase.pages.writepos.crsx > vector)
      xgoto = vbase.pages.writepos.crsx - vector;
    else
      xgoto = 0;
    for (cxidx = vbase.pages.writepos.crsx - 1; cxidx >= xgoto; cxidx--)
      vbase.pages.pagemem[--vbase.pages.writepos.idx] =
          (vbase.pages.curattr << 8) | rchar;
    vbase.pages.writepos.crsx = xgoto;
  } else {
    if (vbase.pages.writepos.crsx + vector < vbase.pages.swidth)
      xgoto = vbase.pages.writepos.crsx + vector;
    else {
      xgoto = (unsigned char)vbase.pages.swidth;
      cursoradjust = 1;
    }
    for (cxidx = vbase.pages.writepos.crsx; cxidx < xgoto; cxidx++)
      vbase.pages.pagemem[vbase.pages.writepos.idx++] =
          (vbase.pages.curattr << 8) | rchar;
    vbase.pages.writepos.idx -= cursoradjust;
    vbase.pages.writepos.crsx = xgoto - cursoradjust;
  }
  vCupDat();
}

void vResetImage(void) {
  memset(vbase.images, 0, sizeof(vbase.images));
  vbase.imemleft = IM_MEMTOTAL / 2;
}

void vReset(void) { vResetImage(); }

int vResetOne(unsigned char box) {
  unsigned short indexs, upperidx, imidx;
  upperidx = IM_MEMTOTAL / 2 - vbase.imemleft;
  if (box >= NUM_IM || !box) return (upperidx);
  box--;
  if (!vbase.images[box].iused) return (upperidx);
  indexs = vbase.images[box].iwidth * vbase.images[box].iheight;
  _fmemmove(&vbase.imagemem[vbase.images[box].idata],
            &vbase.imagemem[vbase.images[box].idata + indexs],
            (IM_MEMTOTAL / 2 - vbase.images[box].idata - indexs) * 2);
  upperidx -= indexs;
  vbase.imemleft += indexs;
  for (imidx = 0; imidx < NUM_IM; imidx++)
    if (vbase.images[box].idata < vbase.images[imidx].idata)
      vbase.images[imidx].idata -= indexs;
  vbase.images[box].iused = 0;
  return (upperidx);
}

void vCurLineToScrap(void) {
  unsigned short lindx;
  lindx = vbase.pages.writepos.idx - vbase.pages.writepos.crsx;
  _fmemcpy(vbase.linemem, &vbase.pages.pagemem[lindx], vbase.pages.swidth * 2);
}

void vScrapToCurLine(void) {
  unsigned short lindx;
  lindx = vbase.pages.writepos.idx - vbase.pages.writepos.crsx;
  _fmemcpy(&vbase.pages.pagemem[lindx], vbase.linemem, vbase.pages.swidth * 2);
}

void vClearCurLine(void) {
  unsigned short lindx, xat;
  lindx = vbase.pages.writepos.idx - vbase.pages.writepos.crsx;
  xat = vbase.pages.swidth;
  while (xat--) vbase.pages.pagemem[lindx++] = vbase.pages.curattr << 8;
}

void vDelCurLine(void) {
  unsigned short lindx;
  lindx = vbase.pages.writepos.idx - vbase.pages.writepos.crsx;
  _fmemmove(&vbase.pages.pagemem[lindx],
            &vbase.pages.pagemem[lindx + vbase.pages.swidth],
            (vbase.pages.numidx - lindx - vbase.pages.swidth) * 2);
  lindx = vbase.pages.numidx - vbase.pages.swidth;
  for (; lindx < vbase.pages.numidx; lindx++)
    vbase.pages.pagemem[lindx] = vbase.pages.curattr << 8;
}

void vInsertCurLine(void) {
  unsigned short lindx, xat;
  lindx = vbase.pages.writepos.idx - vbase.pages.writepos.crsx;
  if (vbase.pages.writepos.crsy < (unsigned char)(vbase.pages.height - 1)) {
    _fmemmove(&vbase.pages.pagemem[lindx + vbase.pages.swidth],
              &vbase.pages.pagemem[lindx],
              (vbase.pages.numidx - lindx - vbase.pages.swidth) * 2);
  }
  xat = vbase.pages.swidth;
  while (xat--) vbase.pages.pagemem[lindx++] = vbase.pages.curattr << 8;
}

void vLineEdit(unsigned char lfunc) {
  switch (lfunc) {
    case 0:
      vInsertBlank();
      return;
    case 1:
      vDeleteCur();
      return;
    case 2:
      vInsertCurLine();
      return;
    case 3:
      vDelCurLine();
      return;
    case 4:
      vCurLineToScrap();
      vDelCurLine();
      return;
    case 5:
      vScrapToCurLine();
      return;
    case 6:
      vInsertCurLine();
      vScrapToCurLine();
      return;
    case 7:
      vCurLineToScrap();
      return;
    case 8:
      vClearCurLine();
      return;
    case 9:
      vClearEOL();
      return;
  }
}

void vInsertBlank(void) {
  unsigned short lastinline, xat;
  lastinline = vbase.pages.writepos.idx - vbase.pages.writepos.crsx +
               vbase.pages.swidth - 1;
  for (xat = vbase.pages.swidth - 1; xat > vbase.pages.writepos.crsx; xat--) {
    vbase.pages.pagemem[lastinline] = vbase.pages.pagemem[lastinline - 1];
    lastinline--;
  }
}

void vDeleteCur(void) {
  unsigned short lastinline, xat;
  lastinline = vbase.pages.writepos.idx;
  for (xat = vbase.pages.writepos.crsx + 1; xat < vbase.pages.swidth; xat++) {
    vbase.pages.pagemem[lastinline] = vbase.pages.pagemem[lastinline + 1];
    lastinline++;
  }
}

void vBorder(unsigned char xsize, unsigned char ysize, unsigned char bstyle) {
  unsigned short xat, yat = 0;
  if (bstyle >= BORDER_TYPES || !ysize || !xsize) return;
  if (xsize + vbase.pages.writepos.crsx > vbase.pages.swidth) /* trim width */
    xsize = (unsigned char)(vbase.pages.swidth - vbase.pages.writepos.crsx);
  if (ysize + vbase.pages.writepos.crsy > vbase.pages.height) /* trim height */
    ysize = (unsigned char)(vbase.pages.height - vbase.pages.writepos.crsy);
  vbase.pages.pagemem[vbase.pages.writepos.idx] = /* u-left corner	0 */
      (vbase.pages.curattr << 8) | borders[bstyle][0];
  for (xat = 1; xat < (unsigned char)(xsize - 1); xat++) /* top bar	1 */
    vbase.pages.pagemem[vbase.pages.writepos.idx + xat] =
        (vbase.pages.curattr << 8) | borders[bstyle][4];
  vbase.pages
      .pagemem[vbase.pages.writepos.idx + xsize - 1] = /* UR		2 */
      (vbase.pages.curattr << 8) | borders[bstyle][1];
  for (xat = 1; xat < (unsigned char)(ysize - 1); xat++) /* l & r sides	3 */
  {
    yat += vbase.pages.swidth;
    vbase.pages.pagemem[vbase.pages.writepos.idx + yat] =
        (vbase.pages.curattr << 8) | borders[bstyle][6];
    vbase.pages.pagemem[vbase.pages.writepos.idx + yat + xsize - 1] =
        (vbase.pages.curattr << 8) | borders[bstyle][7];
  }
  yat += vbase.pages.swidth;
  vbase.pages
      .pagemem[vbase.pages.writepos.idx + yat] = /* l-left		4 */
      (vbase.pages.curattr << 8) | borders[bstyle][2];
  for (xat = 1; xat < (unsigned char)(xsize - 1); xat++) /* bottom bar	1 */
    vbase.pages.pagemem[vbase.pages.writepos.idx + xat + yat] =
        (vbase.pages.curattr << 8) | borders[bstyle][5];
  vbase.pages.pagemem[vbase.pages.writepos.idx + xsize + yat - 1] = /*lr 5*/
      (vbase.pages.curattr << 8) | borders[bstyle][3];
}

void vRepeatVVC(unsigned char vector, unsigned char rchar) {
  unsigned short cyidx, ygoto, cursoradjust = 0;
  if (vector & HIGHBIT) {
    vector &= ~HIGHBIT;
    if (vbase.pages.writepos.crsy > vector)
      ygoto = vbase.pages.writepos.crsy - vector;
    else
      ygoto = 0;
    for (cyidx = vbase.pages.writepos.crsy - 1; cyidx >= ygoto; cyidx--) {
      vbase.pages.writepos.idx -= vbase.pages.swidth;
      vbase.pages.pagemem[vbase.pages.writepos.idx] =
          (vbase.pages.curattr << 8) | rchar;
    }
    vbase.pages.writepos.crsy = ygoto;
  } else {
    if (vbase.pages.writepos.crsy + vector < vbase.pages.height)
      ygoto = vbase.pages.writepos.crsy + vector;
    else {
      ygoto = (unsigned char)vbase.pages.height;
      cursoradjust = 1;
    }
    for (cyidx = vbase.pages.writepos.crsy; cyidx < ygoto; cyidx++) {
      vbase.pages.pagemem[vbase.pages.writepos.idx] =
          (vbase.pages.curattr << 8) | rchar;
      vbase.pages.writepos.idx += vbase.pages.swidth;
    }
    vbase.pages.writepos.idx -= (cursoradjust * vbase.pages.swidth);
    vbase.pages.writepos.crsy = ygoto - cursoradjust;
  }
  vCupDat();
}

void vStatLine(char *message, char position, unsigned char color, char cleol) {
  unsigned short addidx, checkidx;
  addidx = vbase.pages.numidx + position;
  checkidx = vbase.pages.numidx + vbase.pages.swidth;
  while (*message && addidx < checkidx)
    vbase.pages.pagemem[addidx++] = (color << 8) | *message++;
  if (cleol)
    while (addidx < checkidx) vbase.pages.pagemem[addidx++] = (color << 8);
}

void vClearEOL(void) {
  unsigned short aidx, acheck;
  aidx = vbase.pages.writepos.idx - vbase.pages.writepos.crsx;
  acheck = aidx + vbase.pages.swidth;
  aidx += vbase.pages.writepos.crsx;
  while (aidx < acheck)
    vbase.pages.pagemem[aidx++] = (vbase.pages.curattr << 8);
}

void vRepeat(unsigned char rchar, unsigned char numtimes) {
  while (numtimes--) vPutch(rchar);
}

void vCurType(unsigned char newtype) /* not to be called directly */
{
  vbase.pages.curtype = newtype;
  outp(vbase.pages.CRTC, 0xa);
  outp(vbase.pages.CRTC + 1, cur_sizes[newtype][vbase.pages.cardref][0]);
  outp(vbase.pages.CRTC, 0xb);
  outp(vbase.pages.CRTC + 1, cur_sizes[newtype][vbase.pages.cardref][1]);
}

unsigned char vGetHandle(void) /* return first unused image handle */
{
  unsigned char hidx;
  for (hidx = 0; hidx < NUM_IM; hidx++)
    if (!vbase.images[hidx].iused) return ++hidx;
  return 0;
}

void vAttrChange(unsigned char color, unsigned char runlength) {
  unsigned short cxidx, idxgoto, orattr;
  if (vbase.pages.writepos.crsx + runlength >= vbase.pages.swidth)
    idxgoto = (vbase.pages.swidth - vbase.pages.writepos.crsx) +
              vbase.pages.writepos.idx;
  else
    idxgoto = runlength + vbase.pages.writepos.idx;
  orattr = color << 8;
  for (cxidx = vbase.pages.writepos.idx; cxidx < idxgoto; cxidx++)
    vbase.pages.pagemem[cxidx] = orattr | (vbase.pages.pagemem[cxidx] & 0x00ff);
}

void vPage(unsigned char page) {
  if (page < NUMPAGES) {
    vbase.pages = apages[page];
    vbase.curpage = page;
    return;
  }
  if (vbase.curpage) /* don't copy visual onto visual */
    switch (page) {
      case NUMPAGES: /* copy to visual page from current */
                     /* current ---> visual */
        _fmemcpy(apages[0].pagemem, apages[vbase.curpage].pagemem,
                 vbase.pages.numidx * 2);
        break;
      case NUMPAGES + 1: /* copy from visual page to current */
                         /* visual  ---> current */
        _fmemcpy(apages[vbase.curpage].pagemem, apages[0].pagemem,
                 vbase.pages.numidx * 2);
        break;
    }
}

void vLineScroll(unsigned char vector) {
  unsigned short cxidx, xgoto;
  if (vector & HIGHBIT) {
    vector &= ~HIGHBIT;
    if (vector)
      vector--;
    else
      return;
    if (vector + vbase.pages.writepos.crsx >= vbase.pages.swidth)
      vector = vbase.pages.swidth - vbase.pages.writepos.crsx;
    xgoto = vbase.pages.writepos.idx + vector;
    for (cxidx = vbase.pages.writepos.idx; cxidx < xgoto; cxidx++)
      vbase.pages.pagemem[cxidx] = vbase.pages.pagemem[cxidx + 1];
    vbase.pages.pagemem[cxidx] = vbase.pages.curattr << 8;
  } else {
    if (vector + vbase.pages.writepos.crsx >= vbase.pages.swidth)
      vector = vbase.pages.swidth - vbase.pages.writepos.crsx;
    xgoto = vbase.pages.writepos.idx;
    for (cxidx = vbase.pages.writepos.idx + vector; cxidx > xgoto; cxidx--)
      vbase.pages.pagemem[cxidx] = vbase.pages.pagemem[cxidx - 1];
    vbase.pages.pagemem[cxidx] = vbase.pages.curattr << 8;
  }
}

void vWidthHeight(unsigned char *swidth, unsigned char *height) {
  *swidth = (unsigned char)vbase.pages.swidth;
  *height = (unsigned char)vbase.pages.height;
}

void vmode(unsigned short mode) {
  _asm xor ax, ax _asm mov al, byte ptr mode _asm int 0x10
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

/* for VERY FAST output, DOES NOT GO THROUGH standard video calls!
        and is for the LOCAL screen only, does NOT scroll, output must NOT go
   past the end of the display or other memory may be corrupted, also does NOT
        update the cursor position */

void v_sends(unsigned char *in) {
  unsigned short _far *memory = &vbase.pages.pagemem[vbase.pages.writepos.idx];
  while (*in) *memory++ = (vbase.pages.curattr << 8) | *in++;
}

void v_sendsn(unsigned char *in, int length) {
  unsigned short _far *memory = &vbase.pages.pagemem[vbase.pages.writepos.idx];
  vbase.pages.writepos.idx += length;
  while (length--) *memory++ = (vbase.pages.curattr << 8) | *in++;
}

void v_printf(unsigned char *inptr, ...) {
  va_list vargs;
  unsigned char out_buff[17], *ptr, justify, zero, minus, chr;
  unsigned int value, i, width;
  unsigned short _far *memory = &vbase.pages.pagemem[vbase.pages.writepos.idx];
  va_start(vargs, inptr);
  while (chr = *inptr++) {
    if (chr == '%') /* otherwise it's in a seq */
    {               /* format code */
      chr = *inptr++;
      *(ptr = &out_buff[16]) = justify = minus = width = value = i = 0;
      if (chr == '-') { /* left justify */
        --justify;
        chr = *inptr++;
      }
      if (chr == '0') /* leading zeros */
        zero = '0';
      else
        zero = ' ';
      while (chr >= '0' && chr <= '9') { /* field width specifier */
        width *= 10;
        width += (chr - '0');
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
        *memory++ = (vbase.pages.curattr << 8) | '-';
        if (width) --width;
      }
      if (width &&
          !justify) { /* pad with 'zero' value if right justify enabled  */
        i = 0;
        while (ptr[i]) i++;
        for (; i < width; ++i) *memory++ = (vbase.pages.curattr << 8) | zero;
      }
      i = 0; /* move in data */
      value = width - 1;
      while ((*ptr) && (i <= value)) {
        *memory++ = (vbase.pages.curattr << 8) | *ptr++;
        ++i;
      }
      if (width && justify) { /* pad with blanks if left justify enabled */
        while (i < width) {
          *memory++ = (vbase.pages.curattr << 8) | ' ';
          ++i;
        }
      }
    } else
      *memory++ = (vbase.pages.curattr << 8) | chr;
  }
}

char _far *vPageMem(int page) { return ((char _far *)apages[page].pagemem); }
