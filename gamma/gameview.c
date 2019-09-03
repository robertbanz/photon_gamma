#pragma pack(1)
#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "asd.h"
#include "asdmenu.h"
#include "video.h"
typedef unsigned char byte;

#define KEND 34
#define PgUp 73
#define PgDn 81
#define Home 71
#define End 79
#define ESC    \033[

int serport = 3;

void ssends(char *ts) {
  while (*ts) asend(*ts++);
}

struct time {
  byte minute;
  byte hour;
  byte hsecond;
  byte second;
};

struct date {
  int year;
  byte day;
  byte month;
};

struct playerst {
  char passport[10];
  char name[10];
  int score;
};
struct gameview {
  char dummy[3];
  struct tme time;
  struct dte date;
  byte number, field, config, mode1, mode2;
  char redtm1[10], grntm1[10], redtm2[10], grntm2[10];
  struct playerst pl[40];
};
struct gameview gv;

FILE *fp;
long rec, maxrec;

void swap(int i, int j) {
  char tempn[10];
  char tempp[10];
  int temps;
  strncpy(tempn, gv.pl[i].name, 10);
  strncpy(tempp, gv.pl[i].passport, 10);
  temps = gv.pl[i].score;
  strncpy(gv.pl[i].name, gv.pl[j].name, 10);
  strncpy(gv.pl[i].passport, gv.pl[j].passport, 10);
  gv.pl[i].score = gv.pl[j].score;
  strncpy(gv.pl[j].name, tempn, 10);
  strncpy(gv.pl[j].passport, tempp, 10);
  gv.pl[j].score = temps;
}

void SortPlayers(int color) {
  int i, j, minidx;
  if (color == 1) {
    for (i = 0; i < 15; ++i)
      for (j = i + 1; j < 15; ++j)
        if (gv.pl[j].score > gv.pl[i].score) swap(i, j);
    for (i = 20; i < 35; ++i)
      for (j = i + 1; j < 35; ++j)
        if (gv.pl[j].score > gv.pl[i].score) swap(i, j);
  }
}

void ReadRecord(long recnum) {
  fseek(fp, ((long)sizeof(struct gameview) * ((long)recnum - 1L)), SEEK_SET);
  fread(&gv, sizeof(struct gameview), 1, fp);
}

void DisplayRec(void) {
  int i, j, k;
  int rts, gts;
  sChangeAttr(COLOR(HWHT, BLK));
  sPositionCur(0, 3);
  sBox(0xffff, 80, 17, ' ');
  sPositionCur(0, 0);
  sChangeAttr(COLOR(RED, WHT));
  sBox(0xffff, 80, 1, ' ');
  aprintf(" ð No Limit - Photon Database Viewer");
  aprintf("            %05l / ", rec);
  aprintf("%05l", maxrec);
  sPositionCur(0, 1);
  sChangeAttr(COLOR(BLU, BLK));
  sBox(0xffff, 80, 24, '°');
  sChangeAttr(COLOR(HWHT, BLU));
  i = 1;
  j = 2;
  sPositionCur(i, j);

  aprintf("Game Number:");
  sChangeAttr(COLOR(HGRN, BLU));
  aprintf("%3d", gv.number - 1);
  sChangeAttr(COLOR(HWHT, BLU));
  aprintf("      Game Time:");

  aprintf(" %2d:%02d", gv.tm.hour, gv.tm.minute);

  aprintf("      Game Date: ");

  aprintf("%2d/%02d/%4d", gv.dt.month, gv.dt.day, gv.dt.year);

  sPositionCur(0, 2);
  sBox(0xffff, 80, 1, ' ');
  aprintf("                      Game Mode: ");

  if ((gv.field == 1) && (gv.config == 1)) {
    switch (gv.mode1) {
      case 1:
        aprintf("PUBLIC      ");
        break;
      case 2:
        aprintf("LEAGUE      ");
        break;
      case 3:
        aprintf("FREE FOR ALL");
        break;
    }
    sChangeAttr(COLOR(HWHT, BLK));
    sPositionCur(15, 3);
    for (j = 0; j < 10; ++j) asend(gv.redtm1[j]);
    sPositionCur(35, 3);
    for (j = 0; j < 10; ++j) asend(gv.grntm1[j]);
    SortPlayers(1);
    /**DISPLAY GAME**/
    k = 4;
    rts = gts = 0;

    for (i = 0; i < 15; ++i) {
      if (gv.pl[i].passport[0] != 32) {
        sPositionCur(10, (char)++k);

        if ((i == 0) && (gv.pl[0].score > gv.pl[20].score))
          sChangeAttr(COLOR(RED, HBLK));
        else
          sChangeAttr(COLOR(RED, BLK));
        for (j = 0; j < 10; ++j) asend(gv.pl[i].name[j]);
        aprintf("  %5d", gv.pl[i].score);
        rts += gv.pl[i].score;
      }
    }
    k = 4;

    for (i = 20; i < 35; ++i) {
      if (gv.pl[i].passport[0] != 32) {
        sPositionCur(30, (char)++k);
        if ((i == 20) && (gv.pl[0].score < gv.pl[20].score))
          sChangeAttr(COLOR(GRN, HBLK));
        else
          sChangeAttr(COLOR(GRN, BLK));
        for (j = 0; j < 10; ++j) asend(gv.pl[i].name[j]);
        aprintf("  %5d", gv.pl[i].score);
        gts += gv.pl[i].score;
      }
    }
    sChangeAttr(COLOR(HWHT, BLK));
    sPositionCur(21, 19);
    aprintf("%6d", rts);
    sPositionCur(41, 19);
    aprintf("%6d", gts);
  }
}

main(int argc, char *argv[]) {
  int quit;
  char tnum[40];
  if ((fp = fopen(argv[1], "rb")) == NULL) {
    sChangeAttr(COLOR(HRED, BLK));
    ssends("ERROR OPENING DATA FILE!\n");
    exit(1);
  }
  fseek(fp, 0, SEEK_END);
  sPositionCur(0, 0);
  sChangeAttr(COLOR(HWHT, BLK));
  sBox(0xffff, 80, 25, ' ');
  sChangeAttr(COLOR(HWHT, GRN));
  sPositionCur(0, 20);
  sBox(0xffff, 80, 3, ' ');
  ssends("+1 Record  [UpArrow]    +10 Records [PgUp]     Record #1   [Home] ");
  sPositionCur(0, 21);
  ssends("-1 Record  [DnArrow]    -10 Records [PgDn]     Last Record [End] ");
  sPositionCur(0, 22);
  ssends("Jump to Record [Ins]              Quit [Q]");
  maxrec = (ftell(fp) / sizeof(struct gameview));
  rec = 1;
  quit = 0;
  while (quit == 0) {
    ReadRecord(rec);
    DisplayRec();
    while (!(akbhit()))
      ;
    switch ((byte)agetch()) {
      case 1:
        switch ((byte)agetch()) {
          case PGUP:
            if ((rec += 10) > maxrec) rec = maxrec;
            break;
          case PGDOWN:
            if ((rec -= 10) < 1) rec = 1;
            break;
          case UPARROW:
            if (++rec > maxrec) rec = maxrec;
            break;
          case DOWNARROW:
            if (--rec < 1) rec = 1;
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
      case '=':
        if (++rec > maxrec) rec = maxrec;
        break;
      case '-':
        if (--rec < 1) rec = 1;
        break;
      case '1':
        rec = 1;
        break;
      case '0':
        rec = maxrec;
        break;
      case 'q':
      case 'Q':
        quit = 1;
        break;
    }
  }
}
