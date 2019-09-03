
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

#include "asd.h"
#include "asdmenud.h"
#include "gameview.h"
#include "video.h"

char tnum[100];

int ASD_PORT;

struct gdt gd, gd1, gd2;

void ssends(char *ts) {
  while (*ts) asend(*ts++);
}

FILE *gdata;
long rec, maxrec, lastrec;

main() {
  FILE *config;
  char files[10][50];
  char descriptions[10][50];
  int count = 0;
  int i;
  int quit = 1;

  if ((config = fopen("gview.cfg", "rt")) == NULL) {
    printf("\007\007\007Error opening configuration file\n\n\n");
    exit(-1);
  }
  while (!(feof(config))) {
    fscanf(config, "%s", files[count]);
    fscanf(config, "%s", descriptions[count]);
    SpaceOut(descriptions[count]);
    ++count;
  }

  ASDInit(FINDADAPTER);
  while (quit) {
    sPositionCur(0, 0);
    sChangeAttr(COLOR(BLU, WHT));
    sBox(0xffff, 80, 24, 177);
    sChangeAttr(COLOR(RED, WHT));
    sBox(0xffff, 80, 1, ' ');
    asends(" ð BALTIMORE - Game Database Viewer");
    sPositionCur(0, 22);
    sChangeAttr(COLOR(HWHT, GRN));
    sBox(0xffff, 80, 2, ' ');
    asends(" Photon Database Viewer (c) 1992 Robert Banz & Chris Fanning");
    sPositionCur(0, 23);
    asends(" Version 1.5a ");
    mPushMarker();
    for (i = 0; i < count - 1; ++i) mAppendToMenu(descriptions[i], i, 0);
    mAppendToMenu("Quit Gameview", count + 1, 0);
    i = mPopUpMenu(4, 5, NOESC(), 0, count + 2);
    if (i == count + 1)
      quit = 0;
    else
      gameview(files[i]);
  }
  fclose(config);
  exit(-1);
}

int gameview(char *file) {
  int quit = 0;
  ASD_PORT = 3;
  rec = maxrec = lastrec = 0;
  if ((gdata = fopen(file, "rb")) == NULL) {
    printf("ERROR OPENING GAME FILE");
    return (-1);
  }
  fseek(gdata, 0, SEEK_END);
  maxrec = (long)(ftell(gdata) / sizeof(struct gdt));
  InitScreen();
  rec = 1;
  while (quit == 0) {
    if (lastrec != rec) {
      ReadRecord(rec);
      DisplayRecord();
      lastrec = rec;
    }
    while (!(akbhit()))
      ;
    switch (agetch()) {
      case 1:
        switch (agetch()) {
          case PGUP:
            if ((rec += 10) > maxrec) rec = maxrec;
            break;
          case PGDOWN:
            if ((rec -= 10) < 1) rec = 1;
            break;
          case DOWNARROW:
            if (--rec < 1) rec = 1;
            break;
          case UPARROW:
            if (++rec > maxrec) rec = maxrec;
            break;
          case HOME:
            rec = 1;
            break;
          case KEND:
            rec = maxrec;
            break;
          case INSERT:
            *tnum = 0;
            mEditString(tnum, "Record To View: ", 10, 0, 10);
            sscanf(tnum, "%d", &rec);
            if (rec > maxrec)
              rec = maxrec;
            else if (rec < 1)
              rec = 1;
            break;
        }
        break;
      case 'q':
      case 'Q':
        quit = 1;
        break;
    }
  }

  return (0);
}

void InitScreen(void) {
  sPositionCur(0, 0);
  sChangeAttr(COLOR(BLU, BLK));
  sBox(0xffff, 80, 24, ' ');
  sChangeAttr(COLOR(RED, WHT));
  sBox(0xffff, 80, 1, ' ');
  asends(" ð BALTIMORE - Game Database Viewer");
  sPositionCur(0, 22);
  sChangeAttr(COLOR(WHT, BLU));
  sBox(0xffff, 80, 2, ' ');
  asends(" [");
  sEscape(24);
  asends("] +1 Record, [");
  sEscape(25);
  asends("] -1 Record, [PgUp] +10 Records, [PgDn] -10 Records");
  sPositionCur(0, 23);
  asends(" [INS] Enter Record #, [Q] Quit");
  sPositionCur(0, 1);
  sChangeAttr(COLOR(HBLU, BLK));
  asends(" Maneuver:        Date:             Time:           ");
  /*	    012345678901234567890123456789012345678901234567890*/
}

void ReadRecord(long recnum) {
  fseek(gdata, ((long)sizeof(struct gdt) * ((long)recnum - 1L)), SEEK_SET);
  fread(&gd, sizeof(struct gdt), 1, gdata);
}

void DisplayRecord(void) {
  char ts[80];
  sprintf(ts, "%ld/", rec);
  sPositionCur(65, 0);
  sChangeAttr(COLOR(RED, WHT));
  asends(ts);
  sprintf(ts, "%ld     ", maxrec);
  asends(ts);
  sChangeAttr(COLOR(WHT, BLK));
  sPositionCur(0, 2);
  sBox(0xffff, 80, 20, ' ');
  /*PRINT DATE,TIME,NUMBER INFORMATION*/
  sPositionCur(10, 1);
  aprintf("%d   ", gd.number);
  sPositionCur(23, 1);
  aprintf("%02d/%02d/%04d", gd.date.month, gd.date.day, gd.date.year);
  sPositionCur(41, 1);
  aprintf("%02d:%02d", gd.time.hour, gd.time.minute);
  /*if a single field game, display the game in the center*/
  if (gd.field == 1) {
    ScanSafe(&gd);
    DisplayGame(&gd, 20);
  }

  else {
    Copy1Field(&gd, &gd1, 0);
    Copy1Field(&gd, &gd2, 1);
    ScanSafe(&gd1);
    ScanSafe(&gd2);
    DisplayGame(&gd1, 0);
    DisplayGame(&gd2, 40);
  }
}

void DisplayGame(struct gdt *gm, int x) {
  char ts[40];
  int i, j;
  int r, g;
  MakeSafe(gm->redtm1);
  MakeSafe(gm->grntm1);
  sPositionCur(x, 2);
  sChangeAttr(COLOR(HBLU, BLK));
  asends("Game Mode: ");
  if ((gm->mode1 > 0) && (gm->mode1 < 5)) asends(Modes[gm->mode1]);
  sChangeAttr(COLOR(WHT, BLK));
  sPositionCur(x + 5, 4);
  asendc(gm->redtm1, 10);
  sPositionCur(x + 25, 4);
  asendc(gm->grntm1, 10);
  SortGame(gm);
  r = g = 0;
  j = 0;
  for (i = 0; i < 20; ++i) {
    if ((gm->player[0].score > gm->player[20].score) && (i == 0))
      sChangeAttr(COLOR(HRED, BLK));
    else
      sChangeAttr(COLOR(RED, BLK));
    if ((gm->player[i].passport[0] != 32) && (gm->player[i].passport[0])) {
      sPositionCur(x, 6 + j);
      asendc(gm->player[i].name, 10);
      sPositionCur(x + 13, 6 + j);
      aprintf("%5d", gm->player[i].score);
      ++j;
      r += gm->player[i].score;
    }
  }
  j = 0;
  for (i = 20; i < 40; ++i) {
    if ((gm->player[0].score < gm->player[20].score) && (i == 20))
      sChangeAttr(COLOR(HGRN, BLK));
    else
      sChangeAttr(COLOR(GRN, BLK));
    if ((gm->player[i].passport[0] != 32) && (gm->player[i].passport[0])) {
      sPositionCur(x + 20, 6 + j);
      asendc(gm->player[i].name, 10);
      sPositionCur(x + 33, 6 + j);
      aprintf("%5d", gm->player[i].score);
      g += gm->player[i].score;
      ++j;
    }
  }
  if (r > g)
    sChangeAttr(COLOR(HWHT, BLK));
  else
    sChangeAttr(COLOR(WHT, BLK));
  sPositionCur(x + 12, 21);
  aprintf("%6d", r);
  if (g > r)
    sChangeAttr(COLOR(HWHT, BLK));
  else
    sChangeAttr(COLOR(WHT, BLK));
  sPositionCur(x + 32, 21);
  aprintf("%6d", g);
}

void ScanSafe(struct gdt *info) {
  int i;
  for (i = 0; i < 40; ++i) {
    MakeSafe(info->player[i].name);
    MakeSafe(info->player[i].passport);
  }
}

void MakeSafe(char *info) {
  int i;
  for (i = 0; i < 10; ++i) {
    if ((info[i] < 28) && (info[i] > 0)) info[i] = '_';
  }
}

void Copy1Field(struct gdt *source, struct gdt *dest, byte secondf) {
  byte i;
  if (secondf == 1) {
    dest->mode1 = source->mode2;
    memcpy(dest->redtm1, source->redtm2, 10);
    memcpy(dest->grntm1, source->grntm2, 10);
  } else {
    dest->mode1 = source->mode1;
    memcpy(dest->redtm1, source->redtm1, 10);
    memcpy(dest->grntm1, source->grntm1, 10);
  }

  for (i = 0; i < 10; ++i) {
    memcpy(&dest->player[i], &source->player[secondf + (i * 2)],
           sizeof(struct p_type));
    memcpy(&dest->player[i + 20], &source->player[secondf + 20 + (i * 2)],
           sizeof(struct p_type));
    dest->player[i + 10].passport[0] = 32;
    dest->player[i + 30].passport[0] = 32;
  }
}

void SortGame(struct gdt *info) {
  int i, j, minidx;
  for (i = 0; i < 20; ++i)
    for (j = i + 1; j < 20; ++j)
      if (info->player[j].score > info->player[i].score) Swap(info, i, j);
  for (i = 20; i < 40; ++i)
    for (j = i + 1; j < 40; ++j)
      if (info->player[j].score > info->player[i].score) Swap(info, i, j);
}

void Swap(struct gdt *info, int i, int j) {
  struct p_type temp;
  memcpy(&temp, &info->player[i], sizeof(struct p_type));
  memcpy(&info->player[i], &info->player[j], sizeof(struct p_type));
  memcpy(&info->player[j], &temp, sizeof(struct p_type));
}

void SpaceOut(char *info) {
  while (*info != 0) {
    if (*info == '_') *info = ' ';
    ++info;
  }
}
