/***************************************************************************
 *																									*
 *	  ASD Menuing System	Code
 **
 *																									*
 *	  Copyright (c) 1991, Chris Fanning.  All rights reserved.
 ** This file is not for public consumption!
 **
 *																									*
 ***************************************************************************/

#include "asdmenu.h"
#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "asd.h"
#include "asdmenud.h"
#include "video.h"

static struct mPalette stdpalette = {NULL,
                                     2,
                                     COLOR(HBRN, BLU),
                                     COLOR(WHT, BLU),
                                     COLOR(BLK, CYA),
                                     COLOR(CYA, BLU),
                                     COLOR(HCYA, BLU),
                                     COLOR(WHT, BLU),
                                     COLOR(HWHT, PUR),
                                     COLOR(HWHT, PUR),
                                     COLOR(BLK, HWHT),
                                     COLOR(HBRN, RED),
                                     COLOR(HWHT, RED)};

static struct mPalette *lastPalette = &stdpalette;
static struct mItemMarker *lastMarker = NULL;
static struct mItemMarker *savedMarker = NULL;
static struct mPalette *savedPalette = NULL;

static char mPressEscape[] = {"-- Press ESCape to Continue --"};

int mAppendToMenu(char *mtext, int retval, unsigned char hkey) {
  struct mMenuItem *tempitm;
  unsigned int lentext;
  if (!lastMarker) /* no last marker, push one to init */
    if (mPushMarker()) return mNOMEM;
  tempitm = malloc(sizeof(struct mMenuItem));
  if (!tempitm) return (mNOMEM);
  memset(tempitm, 0, sizeof(struct mMenuItem));
  tempitm->prev = lastMarker->last;
  if (!lastMarker->first) /* no items?  ok, make this the first */
    lastMarker->first = tempitm;
  if (lastMarker->last) lastMarker->last->next = tempitm;
  lastMarker->last = tempitm;
  lentext = strlen(mtext); /* get the string length with the NULL */
  if (lentext > MEN_MAXWIDTH - 1) lentext = MEN_MAXWIDTH - 1;
  memcpy(tempitm->text, mtext, lentext);
  if (lentext > lastMarker->widest) lastMarker->widest = lentext;
  tempitm->width = lentext; /* stick the length into the structure */
  tempitm->rval = retval;   /* fill return value for the item */
  tempitm->hotkey = hkey;   /* fill in hotkey value */
  lastMarker->numItems++;   /* increment the number of items in list */
  return (mOK);             /* ciao */
}

int mPushMarker(void) {
  struct mItemMarker *tempitm;
  tempitm = (struct mItemMarker *)malloc(sizeof(struct mItemMarker));
  if (!tempitm) return (mNOMEM);
  memset(tempitm, 0, sizeof(struct mItemMarker));
  tempitm->previous = lastMarker; /* take current marker, make previous */
  lastMarker = tempitm;           /* make new marker the current marker */
  return (mOK);                   /* adios */
}

int mPopMarker(void) {
  struct mMenuItem *tempitm, *atemp;
  struct mItemMarker *amarker;
  if (lastMarker) {
    tempitm = lastMarker->first; /* get the first item */
    amarker = lastMarker->previous;
    free(lastMarker);
    lastMarker = amarker;
  } else
    return mNOMARKER;
  while (tempitm) /* while there ARE items... */
  {
    atemp = tempitm->next; /* assign to temp so it's not lost */
    free(tempitm);         /* free the old one */
    tempitm = atemp;       /* get the next item in the list */
  }
  return (mOK); /* split */
}

int mPopUpMenu(unsigned char xpos, unsigned char ypos, int (*menuaction)(int),
               int itemidx, unsigned char maxshow) {
  struct mMenuItem *tempitem, *curitem;
  int lp, rval, newitemsel, s_keep, s_refresh;
  if (!lastMarker) return mNOMARKER;
  if (lastMarker->numItems == 0) {
    mPopMarker();
    return mNOITEMS;
  }
  if (!maxshow) maxshow = lastMarker->numItems;
  if (itemidx > lastMarker->numItems - 1) itemidx = 0;
  if (lastMarker->numItems < maxshow) maxshow = lastMarker->numItems;
  if (lastMarker->numItems < maxshow)
    lastMarker->mTop = 0;                            /* if all fit... */
  else if (itemidx + maxshow > lastMarker->numItems) /* if beyond end */
    lastMarker->mTop = lastMarker->numItems - maxshow;
  else /* in list so make top the first highlighted */
    lastMarker->mTop = itemidx;

  lastMarker->savedBuff = vGetHandle();
  lastMarker->xWhere = xpos;
  lastMarker->yWhere = ypos;
  lastMarker->maxVisible = maxshow;
  if (lastMarker->previous) mMaskMenu(lastMarker->previous);
  sCurControl(CUR_SAVE);
  sCurShape(CUR_HIDDEN);
  sChangeAttr(lastPalette->menucolor);
  sPositionCur(xpos, ypos);
  sGetImage((unsigned char)(lastMarker->widest + 4),
            (unsigned char)(lastMarker->maxVisible + 2), lastMarker->savedBuff);
  sBox(0xffff, (unsigned char)(lastMarker->widest + 4),
       (unsigned char)(lastMarker->maxVisible + 2), ' ');
  sChangeAttr(lastPalette->menuborder);
  sBorder((unsigned char)(lastMarker->widest + 4),
          (unsigned char)(lastMarker->maxVisible + 2),
          lastPalette->borderstyle);
  sChangeAttr(lastPalette->menucolor);

  /* 28 bytes till there (no prev menu)*/
  curitem = lastMarker->first;
  /* get first menu item */
  for (rval = 0; rval < lastMarker->mTop; rval++) {
    curitem = curitem->next;
    if (!curitem) mAlert("Menu list corrupted.", 10, 5);
  }
  tempitem = curitem;
  for (rval = lastMarker->mTop; rval < itemidx; rval++) curitem = curitem->next;
  /* fill up menu */
  xpos += 2;
  for (rval = 1; rval <= lastMarker->maxVisible; rval++) {
    sPositionCur(xpos, (unsigned char)(ypos + rval));
    asends(tempitem->text);
    tempitem = tempitem->next;
  }

  ypos = lastMarker->yWhere + 1;
  xpos--;
  lastMarker->mWhere = newitemsel = itemidx;

  sPositionCur(xpos,
               (unsigned char)(lastMarker->mWhere - lastMarker->mTop + ypos));
  sChangeBar(lastPalette->menuselect, (unsigned char)(lastMarker->widest + 2));
  rval = mLOOP;
  do {
    switch (agetch()) /* inchar */
    {
      case 0:
        agetch();
        break;
      case 27:
        if (menuaction)
          rval = menuaction(pESC);
        else
          rval = pESC;
        break;
      case 13:
        if (menuaction)
          rval = menuaction(curitem->rval);
        else
          rval = curitem->rval;
        break;
      case 1:
        switch (agetch()) {
          case HOME:
            curitem = lastMarker->first;
            newitemsel = 0;
            break;
          case KEND:
            curitem = lastMarker->last;
            newitemsel = lastMarker->numItems - 1;
            break;
          case UPARROW:
            if (curitem->prev) {
              newitemsel = lastMarker->mWhere - 1;
              curitem = curitem->prev;
            }
            break;
          case DOWNARROW:
            if (curitem->next) {
              newitemsel = lastMarker->mWhere + 1;
              curitem = curitem->next;
            }
            break;
        } /* end of ASD extended switch */
          /*********************************************************
                                  default:
                                          go through all letters for hotkey... */
    }     /* end of inchar switch */
    if (lastMarker->mWhere != newitemsel) {
      sPositionCur(
          xpos, (unsigned char)(lastMarker->mWhere - lastMarker->mTop + ypos));
      sChangeBar(lastPalette->menucolor,
                 (unsigned char)(lastMarker->widest + 2));
      if (newitemsel < lastMarker->mTop) /* scroll down & fill top */
      {
        s_keep = lastMarker->maxVisible - (lastMarker->mTop - newitemsel);
        if (s_keep < 0) s_keep = 0;
        s_refresh = lastMarker->maxVisible - s_keep;
        lastMarker->mTop = newitemsel;
        if (s_keep) {
          sPositionCur(xpos, ypos);
          sGetImage((unsigned char)(lastMarker->widest + 1),
                    (unsigned char)s_keep, 30);
          sPositionCur(xpos, (unsigned char)(ypos + s_refresh));
          sPutImage(30);
        }
        if (s_refresh) {
          xpos++;
          tempitem = curitem->next;
          sPositionCur(xpos, ypos);
          sBox(CHARMASK | ATTRMASK, lastMarker->widest,
               (unsigned char)s_refresh, ' ');
          asends(curitem->text);
          for (lp = 1; lp < s_refresh; lp++) {
            sPositionCur(xpos, (unsigned char)(ypos + lp));
            asends(tempitem->text);
            tempitem = tempitem->next;
          }
          xpos--;
        }
      } else if (newitemsel >=
                 lastMarker->mTop +
                     lastMarker->maxVisible) { /* scroll up & fill bottom */
        s_refresh = newitemsel - (lastMarker->mTop + lastMarker->maxVisible);
        if (++s_refresh > lastMarker->maxVisible)
          s_refresh = lastMarker->maxVisible;
        s_keep = lastMarker->maxVisible - s_refresh;
        lastMarker->mTop = newitemsel - lastMarker->maxVisible + 1;
        if (s_keep) {
          sPositionCur(xpos, (unsigned char)(ypos + s_refresh));
          sGetImage((unsigned char)(lastMarker->widest + 1),
                    (unsigned char)s_keep, 30);
          sPositionCur(xpos, ypos);
          sPutImage(30);
        }
        if (s_refresh) {
          xpos++;
          tempitem = curitem;
          sPositionCur(xpos, (unsigned char)(ypos + s_keep));
          sBox(CHARMASK | ATTRMASK, lastMarker->widest,
               (unsigned char)s_refresh, ' ');
          for (lp = 1; lp <= s_refresh; lp++) {
            sPositionCur(xpos,
                         (unsigned char)(ypos + (lastMarker->maxVisible - lp)));
            asends(tempitem->text);
            tempitem = tempitem->prev;
          }
          xpos--;
        }
      }
      lastMarker->mWhere = newitemsel;
      sPositionCur(
          xpos, (unsigned char)(lastMarker->mWhere - lastMarker->mTop + ypos));
      sChangeBar(lastPalette->menuselect,
                 (unsigned char)(lastMarker->widest + 2));
    }
  } while (rval == mLOOP);
  mDestroyMenu();
  if (rval != pESC)
    return (curitem->rval);
  else
    return rval;
}

int mDestroyMenu(void) {
  if (!lastMarker->savedBuff) return (mNOMENU);
  sPositionCur(lastMarker->xWhere, lastMarker->yWhere);
  sPutImage(lastMarker->savedBuff);
  sFreeHandle(lastMarker->savedBuff);
  sCurControl(CUR_RESTORE);
  mPopMarker();
  if (lastMarker) mUnMaskMenu(lastMarker);
  return (mOK);
}

int mMaskMenu(struct mItemMarker *dimmenu) {
  if (!dimmenu->savedBuff) return (mNOMENU);
  sCurControl(CUR_SAVE);
  sChangeAttr(lastPalette->menudim);
  sPositionCur(dimmenu->xWhere, dimmenu->yWhere);
  sBox(0xff00, (unsigned char)(dimmenu->widest + 4),
       (unsigned char)(dimmenu->maxVisible + 2), ' ');
  sCurControl(CUR_RESTORE);
  return (mOK); /* 15 bytes total ... ohmygosh! */
}

void mUnMaskMenu(struct mItemMarker *dimmenu) {
  sChangeAttr(lastPalette->menucolor);
  sPositionCur(dimmenu->xWhere, dimmenu->yWhere);
  sBox(ATTRMASK, (unsigned char)(dimmenu->widest + 4),
       (unsigned char)(dimmenu->maxVisible + 2), ' ');
  sChangeAttr(lastPalette->menuborder);
  sBorder((unsigned char)(dimmenu->widest + 4),
          (unsigned char)(dimmenu->maxVisible + 2), lastPalette->borderstyle);
  sChangeAttr(lastPalette->menucolor);
  sPositionCur(
      (unsigned char)(dimmenu->xWhere + 1),
      (unsigned char)(dimmenu->mWhere - dimmenu->mTop + dimmenu->yWhere + 1));
  sChangeBar(lastPalette->menuselect, (unsigned char)(dimmenu->widest + 2));
}

void mPushPalette(struct mPalette *newpal) {
  newpal->previous = lastPalette;
  lastPalette = newpal;
}

void mPopPalette(void) {
  if (lastPalette->previous) lastPalette = lastPalette->previous;
}

int mAlert(char *message, unsigned char xpos, unsigned char ypos) {
  int lens, lene;
  unsigned char handle;
  lens = strlen(message);
  lene = strlen(mPressEscape);
  if (lens < lene) lens = lene;
  sCurControl(CUR_SAVE);
  sCurShape(0);
  sChangeAttr(78);
  sPositionCur((unsigned char)xpos, (unsigned char)ypos);
  sGetImage((unsigned char)(lens + 4), 4, handle = vGetHandle());
  sBox(0xffff, (unsigned char)(lens + 4), 4, ' ');
  sBorder((unsigned char)(lens + 4), 4, 2);
  mCenterText((unsigned char)(xpos + 2), (unsigned char)(ypos + 1),
              (unsigned char)(lens), message);
  mCenterText((unsigned char)(xpos + 2), (unsigned char)(ypos + 2),
              (unsigned char)(lens), mPressEscape);
  do {
    lens = agetch();
    if (lens <= 1) agetch();
  } while (lens != 27);
  sPositionCur((unsigned char)xpos, (unsigned char)ypos);
  sPutImage(handle);
  sFreeHandle(handle);
  sCurControl(CUR_RESTORE);
  return mOK;
}

void mCenterText(unsigned char xwhere, unsigned char ywhere,
                 unsigned char xlength, char *message) {
  int lens, diff, toput, lp;
  lens = strlen(message);
  if (lens > xlength)
    toput = xlength;
  else
    toput = lens;
  diff = xlength - lens;
  if (diff < 0) diff = 0;
  diff /= 2;
  sPositionCur((unsigned char)(xwhere + diff), ywhere);
  for (lp = 0; lp < toput; lp++) asend(message[lp]);
}

int mEString(char *buffer, unsigned char length, unsigned char resume) {
  unsigned int eos, inchar, swhere, scrap, insmode = 1;
  char *old;
  sCurControl(CUR_SAVE);
  sCurShape(CUR_UNDER);
  swhere = strlen(buffer);
  if (swhere > (unsigned int)length) length = (unsigned char)swhere;
  eos = swhere;
  if (swhere) {
    old = (char *)malloc(swhere + 1);
    if (old)
      strcpy(old, buffer);
    else
      old = NULL;
  } else
    old = NULL;
  if (!resume) {
    sRepeatChar(' ', length);
    sCurBack(length);
    asends(buffer);
  } else
    sCurForward((unsigned char)swhere);
  do {
    inchar = agetch();
    switch (inchar) {
      case 0:
        agetch();
        break;
      case CR:
        buffer[eos] = 0;
        if (old) free(old);
        sCurControl(CUR_RESTORE);
        return eos;
      case 27:
        if (old) {
          strcpy(buffer, old);
          free(old);
          sCurControl(CUR_RESTORE);
          return swhere;
        } else
          buffer[0] = 0;
        sCurControl(CUR_RESTORE);
        return 0;
      case BACKS:
        if (!swhere) break;
        swhere--;
        eos--;
        if (swhere == eos) {
          asend(BACKS);
          asend(' ');
          asend(BACKS);
        } else {
          scrap = length - swhere;
          asend(BACKS);
          sScrollLeft((unsigned char)scrap);
          memmove(&buffer[swhere], &buffer[swhere + 1], scrap);
        }
        break;
      case 1: /* ASD extended chars */
        scrap = agetch();
        switch (scrap) {
          case INSERT:
            if (insmode) {
              insmode = 0;
              sCurShape(CUR_FULL);
            } else {
              insmode++;
              sCurShape(CUR_UNDER);
            }
            break;
          case HOME:
            sCurBack(swhere);
            swhere = 0;
            break;
          case KEND:
            swhere = eos - swhere;
            sCurForward(swhere);
            swhere = eos;
            break;
          case LEFTARROW:
            if (swhere) {
              swhere--;
              asend(BACKS);
            }
            break;
          case RIGHTARROW:
            if (swhere < eos) {
              sCurForward(1);
              swhere++;
            }
            break;
          case DELETE:
            if (eos != swhere) {
              eos--;
              scrap = length - swhere;
              sScrollLeft((unsigned char)scrap);
              memmove(&buffer[swhere], &buffer[swhere + 1], scrap);
            }
        } /* end of extended switch */
        break;
      default:
        if (insmode) {
          if (eos < length) {
            if (eos != swhere) {
              scrap = length - swhere;
              sScrollRight((unsigned char)scrap);
              memmove(&buffer[swhere + 1], &buffer[swhere], scrap);
            }
            buffer[swhere] = (char)inchar;
            swhere++;
            eos++;
            asend((unsigned char)inchar);
          }
        } else {
          if (swhere < length) {
            asend((unsigned char)inchar);
            buffer[swhere] = (char)inchar;
            swhere++;
            if (swhere > eos) eos = swhere;
          }
        }
    } /* end switch */
  } while (1);
}

void mResetMenu(void) {
  struct mPalette *tempPalette;
  while (lastMarker) {
    sFreeHandle(lastMarker->savedBuff);
    mPopMarker();
  }
  while (lastPalette != &stdpalette) {
    tempPalette = lastPalette->previous;
    free(lastPalette);
    lastPalette = tempPalette;
  }
}

int mEditString(char *buffer, char *prompt, unsigned char length,
                unsigned char xpos, unsigned char ypos) {
  int lprompt;
  unsigned char handle;
  if (prompt)
    lprompt = strlen(prompt);
  else
    lprompt = 0;
  sCurControl(CUR_SAVE);
  sCurShape(CUR_UNDER);
  sChangeAttr(lastPalette->editcolor);
  sPositionCur(xpos, ypos);
  sGetImage((unsigned char)(length + lprompt + 3), 3, handle = vGetHandle());
  sBox(0xffff, (unsigned char)(length + lprompt + 3), 3, ' ');
  sChangeAttr(lastPalette->editborder);
  sBorder((unsigned char)(length + lprompt + 3), 3, 2);
  sChangeAttr(lastPalette->editcolor);
  sPositionCur((unsigned char)(xpos + 1), (unsigned char)(ypos + 1));
  if (lprompt) asends(prompt);
  sPositionCur((unsigned char)(xpos + lprompt + 1), (unsigned char)(ypos + 1));
  lprompt = mEString(buffer, length, 0);
  sPositionCur(xpos, ypos);
  sPutImage(handle);
  sFreeHandle(handle);
  sCurControl(CUR_RESTORE);
  return lprompt;
}

int mYesNo(char *prompt, unsigned char xpos, unsigned char ypos, int mdefault) {
  int lprompt, rval = 1, inchar;
  unsigned char handle, ynwhere;
  lprompt = strlen(prompt);
  lprompt += 4; /* add the borders to the length */
  if (lprompt < 14) lprompt = 14;
  ynwhere = lprompt / 2 + xpos - 5;
  if (lastMarker && lastMarker->numItems) mMaskMenu(lastMarker);
  sCurControl(CUR_SAVE);
  sCurShape(CUR_HIDDEN);
  sChangeAttr(lastPalette->yesnocolor);
  sPositionCur(xpos, ypos);
  sGetImage((unsigned char)lprompt, 4, handle = vGetHandle());
  sBox(0xffff, (unsigned char)(lprompt), 4, ' ');
  sChangeAttr(lastPalette->yesnoborder);
  sBorder((unsigned char)lprompt, 4, 2);
  sChangeAttr(lastPalette->yesnocolor);
  mCenterText(xpos, (unsigned char)(ypos + 1), (unsigned char)lprompt, prompt);
  sPositionCur(ynwhere, (unsigned char)(ypos + 2));
  asends(" Yes  No");
  if (!mdefault) {
    ynwhere += 5;
    rval = 0;
  }
  do {
    sPositionCur(ynwhere, (unsigned char)(ypos + 2));
    sChangeBar(lastPalette->yesnoselect, 5);
    inchar = agetch();
    if (!inchar) agetch();
    if (inchar == 1) {
      switch (agetch()) {
        case LEFTARROW:
        case RIGHTARROW:
          rval ? (ynwhere += 5) : (ynwhere -= 5);
          rval ? (rval = 0) : (rval = 1);
          break;
      }
      sChangeBar(lastPalette->yesnocolor, 5);
    }
  } while (inchar != 13);
  sPositionCur(xpos, ypos);
  sPutImage(handle);
  sFreeHandle(handle);
  if (lastMarker && lastMarker->numItems) mUnMaskMenu(lastMarker);
  sCurControl(CUR_RESTORE);
  return rval;
}

int NOESC(int select) {
  if (select == pESC)
    return mLOOP;
  else
    return select;
}
