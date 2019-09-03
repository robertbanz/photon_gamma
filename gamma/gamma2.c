/*************
 gamma2.c
 *************/
#define SYSTEM_PROGRAM

#undef __TOPLEV
#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include "asd.h"
#include "asddat.h"
#include "gamma.h"
#include "outctl.h"
#include "video.h"
#ifdef EXTENDED_GRAPHICS
#include <graph.h>
#include "fontop.h"
#include "g_asd.h"
extern ASD_COMP ASDcore;
#endif

#define CRED 4

#pragma optimize("", off)
void ReadXfer(unsigned int etport) {
  byte m;
  int n;
  int length;
  length = 1 + 20 + (20 * 10) + (20 * 10) + 1;
  WaitAPoll();
  outp(etport, 0xD2);

  while (poll_flag == FALSE)
    ;
  poll_flag = FALSE;
  while (poll_flag == FALSE)
    ;
  poll_flag = FALSE;
  if (ET_CheckDrop(etport)) return;
  n = 0;
  m = 0;
  m = inp(etport + 5);
  outp(etport, SENDRED);
  for (n = 0; n < length; ++n) {
    if (ET_CheckDrop(etport)) return;
    do
      m = inp(etport + 2);
    while (m != 4);
    m = inp(etport);
    transfer[0][n] = m;
  }
  WaitAPoll();
  outp(etport, 0x15);
  WaitAPoll();
  outp(etport, 0xD2);
  WaitAPoll();
  outp(etport, SENDGRN);
  n = 0;
  m = 0;
  for (n = 0; n < length; ++n) {
    if (ET_CheckDrop(etport)) return;
    do
      m = inp(etport + 2);
    while (m != 4);
    m = inp(etport);
    transfer[1][n] = m;
  }
  WaitAPoll();
  outp(etport, 0x06);
  WaitAPoll();
  outp(etport, 0xd2);
  TranslateXfer(etport);
}
#pragma optimize("", on)

byte ET_CheckDrop(unsigned int etport) {
  struct com_6 modeminfo;
  ReadPort(etport + 6, &modeminfo);
  if ((modeminfo.CTS != 1) || (modeminfo.DSR != 1)) {
    if (mYesNo("ET Signal Lost ... Reconfigure ???", 10, 10, 1)) {
      if (etport == ET1)
        ET_config1(FALSE);
      else
        ET_config2(FALSE);
      return (1);
    } else {
      ReadPort(etport + 6, &modeminfo);
      ResetIBM();
      exit(-1);
    }
  }
  return (0);
}

void SetGameMode(byte *mode, byte source) {
  if (source == 0xE1)
    *mode = PUBLIC;
  else if (source == 0xE2)
    *mode = LEAGUE;
  else if (source == 0xE3)
    *mode = FREEFORALL;
}

void settrack(void) {
  extern byte gametrack;
  char string[20];

  sprintf(string, "RU%d", gametrack);

  effectsout(string, strlen(string));
}

void setupslots() {
  irslot[1][RED] = 0x42;
  irslot[2][RED] = 0x43;
  irslot[3][RED] = 0x44;
  irslot[4][RED] = 0x45;
  irslot[5][RED] = 0x46;
  irslot[6][RED] = 0x47;
  irslot[7][RED] = 0x48;
  irslot[8][RED] = 0x49;
  irslot[9][RED] = 0x4a;
  irslot[10][RED] = 0x4b;
  irslot[11][RED] = 0x4c;
  irslot[12][RED] = 0x4d;
  irslot[13][RED] = 0x4e;
  irslot[14][RED] = 0x4f;
  irslot[15][RED] = 0x50;
  irslot[16][RED] = 0x51;
  irslot[17][RED] = 0x52;
  irslot[18][RED] = 0x53;
  irslot[19][RED] = 0x54;
  irslot[20][RED] = 0x55;
  irslot[1][GREEN] = 0x94;
  irslot[2][GREEN] = 0x81;
  irslot[3][GREEN] = 0x82;
  irslot[4][GREEN] = 0x83;
  irslot[5][GREEN] = 0x84;
  irslot[6][GREEN] = 0x85;
  irslot[7][GREEN] = 0x86;
  irslot[8][GREEN] = 0x87;
  irslot[9][GREEN] = 0x88;
  irslot[10][GREEN] = 0x89;
  irslot[11][GREEN] = 0x8a;
  irslot[12][GREEN] = 0x8b;
  irslot[13][GREEN] = 0x8c;
  irslot[14][GREEN] = 0x8d;
  irslot[15][GREEN] = 0x8e;
  irslot[16][GREEN] = 0x8f;
  irslot[17][GREEN] = 0x90;
  irslot[18][GREEN] = 0x91;
  irslot[19][GREEN] = 0x92;
  irslot[20][GREEN] = 0x93;
  irslot[1][TEST] = 0x41;
  irslot[2][TEST] = 0x41;
  slotir[0x42] = 1;
  slotir[0x43] = 2;
  slotir[0x44] = 3;
  slotir[0x45] = 4;
  slotir[0x46] = 5;
  slotir[0x47] = 6;
  slotir[0x48] = 7;
  slotir[0x49] = 8;
  slotir[0x4a] = 9;
  slotir[0x4b] = 10;
  slotir[0x4c] = 11;
  slotir[0x4d] = 12;
  slotir[0x4e] = 13;
  slotir[0x4f] = 14;
  slotir[0x50] = 15;
  slotir[0x51] = 16;
  slotir[0x52] = 17;
  slotir[0x53] = 18;
  slotir[0x54] = 19;
  slotir[0x55] = 20;
  slotir[0x94] = 20 + 1;
  slotir[0x81] = 20 + 2;
  slotir[0x82] = 20 + 3;
  slotir[0x83] = 20 + 4;
  slotir[0x84] = 20 + 5;
  slotir[0x85] = 20 + 6;
  slotir[0x86] = 20 + 7;
  slotir[0x87] = 20 + 8;
  slotir[0x88] = 20 + 9;
  slotir[0x89] = 20 + 10;
  slotir[0x8a] = 20 + 11;
  slotir[0x8b] = 20 + 12;
  slotir[0x8c] = 20 + 13;
  slotir[0x8d] = 20 + 14;
  slotir[0x8e] = 20 + 15;
  slotir[0x8f] = 20 + 16;
  slotir[0x90] = 20 + 17;
  slotir[0x91] = 20 + 18;
  slotir[0x92] = 20 + 19;
  slotir[0x93] = 20 + 20;
  slotir[0x41] = 40 + 1;
}

#ifndef EXTENDED_GRAPHICS
void LOBBY_boot(void) {
  char ts[80];
  SelectIO(&PUB);
  sPositionCur(0, 0);
  sChangeAttr(COLOR(BLK, WHT));
  vStatLine("", 0, COLOR(BLK, WHT), 1);
  fBox(0xffff, ' ', 40, 24);

  sChangeAttr(COLOR(HWHT, BLU));
  fBox(0xffff, ' ', 40, 4);

  sPositionCur(1, 0);
  asends("Stimpy, sometimes your wealth of");
  sPositionCur(1, 1);
  asends("ignorance astounds me...this MUST");
  sPositionCur(1, 2);
  sprintf(ts, "be the Gamma Systems Computer V%s", SYSTEMVERSION);
  asends(ts);
  sChangeAttr(COLOR(BLK, WHT));
  sPositionCur(1, 6);
  asends("Photon Systems Software");
  sPositionCur(1, 8);
  asends("(c) 1991,92,93 Robert Banz and");
  sPositionCur(10, 9);
  asends("Chris Fanning.");
  sPositionCur(10, 10);
  asends("All Rights Reserved");
  sPositionCur(1, 12);
  asends("Licensed to Quantum Entertainment Inc.");
  sPositionCur(1, 14);
  asends(__DATE__);
  sPositionCur(1, 16);
  asends("The system is coming up, please wait.");
  SelectIO(&CONS);
}
#else

void LOBBY_boot(void) {
  char *m;
  m = (char *)malloc(100);

  sxToGraphics(_VRES16COLOR);
  sxColorSelect(7, 0);
  ReadScreen("screens\\logo.asd");
  sxColorSelect(5, 0);
  sxDrawLine(0, 125, 639, 125);
  sxFloodFill(1, 176);
  sxColorSelect(0, 0);
  sxFontSelect(1);
  sxPrintText(1, INCR | 1,
              "You knew it was coming, you just didn't know when!");
  sxPrintText(1, INCR | 1,
              "(c) 1991-93 Robert Banz & Chris Fanning, All Rights Reserved");
  sxPrintText(1, INCR | 2, "Special Thanks To: John Shappert & Chris Hutson.");
  sprintf(m, "Date Compiled: %s, Version V%s.", __DATE__, SYSTEMVERSION);
  sxPrintText(1, INCR | 2, m);
  sxPrintText(CEN, INCR | 2, "The system is coming up, please wait.");

  free(m);
}

#endif
#ifdef EXTENDED_GRAPHICS
void ReadScreen(char *file) {
  FILE *scr;

  if ((scr = fopen(file, "rb")) == 0) {
    return;
    exit(1);
  }

  while (!feof(scr)) {
    ASDcore.ASDput(fgetc(scr));
  }
  fclose(scr);
}
#endif
void PrintMessage(int dummy) {
  int x, y;
  FILE *fp;
  char filename[20];
  char ts[90];
  char tempstring[50];

#ifndef EXTENDED_GRAPHICS
  sprintf(filename, "%smess.%d", curconfig.mess_path, tnum);
  if ((fp = fopen(filename, "r")) != NULL) {
    fgets(tempstring, 40, fp);
    getc(fp);
    getc(fp);
    getc(fp);
    sChangeAttr(COLOR(BLK, WHT));
    sPositionCur(0, 3);
    fBox(0xffff, ' ', 40, 10);
    for (y = 0; y < 10; ++y) {
      fgets(textinfo[y], 40, fp);
      nl_to_null(textinfo[y]);
      fgetc(fp);
      sPositionCur(0, (byte)(y + 3));
      asends(textinfo[y]);
    }
  }
  fclose(fp);
  if (tnum == 10) tnum = 0;
  SelectIO(&CONS);
#else
#endif
}

char *TitleString = "Photon Systems Computer ALPHA_01";

void LOBBY_tween() {
  int x, y;
  char ts[90];
  char *SSS;

#ifndef EXTENDED_GRAPHICS
  SelectIO(&PUB);
#ifdef EXTENDED_GRAPHICS
  sxToText();
#endif
  sChangeAttr(COLOR(BLK, WHT));
  sPositionCur(0, 0);
  fBox(0xffff, ' ', 40, 24);
  sChangeAttr(COLOR(HWHT, BLU));
  fBox(0xffff, ' ', strlen(TitleString) + 2, 2);
  asends(TitleString);
  sChangeAttr(COLOR(BLK, WHT));
  sPositionCur(0, 2);
  vRepeat('Í', strlen(TitleString) + 2);
  for (i = 0; i < 2; ++i) {
    sPositionCur(strlen(TitleString) + 2, i);
    vPutch('³');
  }
  sPositionCur(strlen(TitleString) + 2, 2);
  vPutch('¾');
  sPositionCur(0, 13);
  sBorder(40, 8, 2);
  sPositionCur(1, 13);
  sChangeAttr(COLOR(WHT, BLU));
  vRepeat('ß', 38);
  sPositionCur(1, 20);
  vRepeat('Ü', 38);
  sPositionCur(1, 14);
  fBox(0xffff, ' ', 38, 6);
  sChangeAttr(COLOR(BLK, WHT));
  sPositionCur(1, 21);
  asends("Photon Time");
  sPositionCur(26, 21);
  asends("Next Manuever");
  sPositionCur(30, 22);
  aprintf("%04d", game.number);
  sChangeAttr(COLOR(BLK, WHT));
  sPositionCur(4, 22);
  sprintf(ts, "%2d:%02d", (int)curtime.hour, (int)curtime.minute);
  asends(ts);
  PrintMessage(++tnum);
  SelectIO(&PUB);
  /*print the high scores in the high score zone*/
  sChangeAttr(COLOR(HBRN, BLU));
  sPositionCur(2, 14);
  asends("High score of the week:");
  sPositionCur(2, 17);
  asends("Highest hits of the week:");
  sChangeAttr(COLOR(HWHT, BLU));
  sPositionCur(7, 15);
  if (weekly.highscore.passport[0]) {
    asendc(weekly.highscore.name, 10);
    sPositionCur(23, 15);
    sprintf(ts, "%6d", weekly.highscore.score);
    asends(ts);
  } else
    asends("None yet!  It could be YOU!");
  sPositionCur(7, 18);
  if (weekly.highhits.passport[0]) {
    asendc(weekly.highscore.name, 10);
    sPositionCur(23, 18);
    aprintf("%3d", weekly.highhits.hits);
  } else
    asends("None yet!  It could be YOU!");
  SelectIO(&CONS);
#else

  sxClear();
  sxColorSelect(1, 0);
  sxDrawLine(0, 30, 649, 30);
  sxFloodFill(1, 1);
  sxColorSelect(15, 0);
  sxFontSelect(0);
  sxPrintText(CEN, 0, TitleString);

#endif
}

void LOBBY_instokn() {
  int y;
  char ts[80];

#ifdef EXTENDED_GRAPHICS
  sxToText();
#endif

  SelectIO(&PUB);
  sChangeAttr(COLOR(BLK, WHT));
  sPositionCur(0, 0);
  fBox(0xffff, ' ', 40, 24);
  sChangeAttr(COLOR(HWHT, BLU));
  fBox(0xffff, ' ', strlen(TitleString) + 2, 2);
  asends(TitleString);
  sChangeAttr(COLOR(BLK, WHT));
  sPositionCur(0, 2);
  vRepeat('Í', strlen(TitleString) + 2);
  for (i = 0; i < 2; ++i) {
    sPositionCur(strlen(TitleString) + 2, i);
    vPutch('³');
  }
  sPositionCur(strlen(TitleString) + 2, 2);
  vPutch('¾');
  sPositionCur(0, 13);
  sBorder(40, 8, 2);
  sPositionCur(1, 13);
  sChangeAttr(COLOR(WHT, BLU));
  vRepeat('ß', 38);
  sPositionCur(1, 20);
  vRepeat('Ü', 38);
  sPositionCur(1, 14);
  fBox(0xffff, ' ', 38, 6);
  sChangeAttr(COLOR(BLK, WHT));
  sPositionCur(1, 21);
  asends("Photon Time");
  sPositionCur(26, 21);
  asends("Next Manuever");
  sPositionCur(30, 22);
  aprintf("%04d", game.number);
  sChangeAttr(COLOR(BLK, WHT));
  sPositionCur(4, 22);
  sprintf(ts, "%2d:%02d", (int)curtime.hour, (int)curtime.minute);
  asends(ts);
  PrintMessage(((tnum == 0) ? tnum++ : tnum));
  SelectIO(&PUB);
  sPositionCur(2, 14);
  sChangeAttr(COLOR(BLK, WHT));
  fBox(0xffff, ' ', 36, 6);
  sChangeAttr(COLOR(CRED, HWHT));
  sPositionCur(3, 14);
  asends("*ATTENTION:*");
  sChangeAttr(COLOR(BLK, WHT));
  sPositionCur(16, 14);
  asends("Maneuver in final");
  sPositionCur(4, 15);
  asends("phase of staging.");
  sPositionCur(4, 18);
  asends("Next maneuver please report to");
  sPositionCur(4, 19);
  asends("staging area now !");

  /*DispGameModes();*/
  SelectIO(&CONS);
}

void LOBBY_noxfer() {
  SelectIO(&PUB);
  sChangeAttr(COLOR(HWHT, BLU));
  vStatLine("", 0, COLOR(HWHT, BLU), 1);
  SelectIO(&CONS);
}
void Dig_Digit(int digit, byte x, byte y) {
  int i;
  static char data[10][7] = {{1, 1, 1, 1, 1, 1, 0}, {0, 1, 1, 0, 0, 0, 0},
                             {1, 1, 0, 1, 1, 0, 1}, {1, 1, 1, 1, 0, 0, 1},
                             {0, 1, 1, 0, 0, 1, 1}, {1, 0, 1, 1, 0, 1, 1},
                             {1, 0, 1, 1, 1, 1, 1}, {1, 1, 1, 0, 0, 0, 0},
                             {1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 0, 1, 1}};
  static char s[7][4] = {{1, 0, 4, 1}, {5, 1, 1, 2}, {5, 4, 1, 2}, {1, 6, 4, 1},
                         {0, 4, 1, 2}, {0, 1, 1, 2}, {1, 3, 4, 1}};
  sPositionCur(x, y);
  fBox(0xffff, ' ', 6, 7);

  if ((digit >= 0) && (digit < 10)) {
    for (i = 0; i < 7; ++i) {
      if (data[digit][i]) {
        sPositionCur(x + s[i][0], y + s[i][1]);
        fBox(0xffff, 219, s[i][2], s[i][3]);
      }
    }
  }
}

void LOBBY_alert() {
  char ts[80];
  int i;
  char *alert[4] = {
      "      ÜßßÜ Û   Ûßßß ÛßßÜ ßßÛßß", "  þ   Û  Û Û   Û    Û  Û   Û     þ",
      "  þ   ÛßßÛ Û   Ûßß  ÛßßÜ   Û     þ", "      Û  Û ÛÜÜ ÛÜÜÜ Û  Û   Û"};

#ifdef EXTENDED_GRAPHICS
  sxToText();
#endif

  SelectIO(&PUB);
  sChangeAttr(COLOR(HWHT, BLK));
  sPositionCur(0, 0);
  fBox(0xffff, ' ', 40, 24);
  sChangeAttr(COLOR(HWHT, BLU));
  fBox(0xffff, ' ', strlen(TitleString) + 2, 2);
  asends(TitleString);
  sChangeAttr(COLOR(HWHT, BLK));
  sPositionCur(0, 2);
  vRepeat('Í', strlen(TitleString) + 2);
  for (i = 0; i < 2; ++i) {
    sPositionCur(strlen(TitleString) + 2, i);
    vPutch('³');
  }
  sPositionCur(strlen(TitleString) + 2, 2);
  vPutch('¾');
  sChangeAttr(COLOR(BLU, BLK));
  sPositionCur(7, 6);
  vPutch('ß');
  sPositionCur(8, 6);
  vPutch('Û');
  sPositionCur(9, 6);
  vPutch('ß');
  sPositionCur(8, 7);
  vPutch('Û');
  sPositionCur(8, 8);
  vPutch('ß');
  sPositionCur(11, 7);
  asends("ßß");
  sChangeAttr(COLOR(HWHT, BLK));
  sPositionCur(0, 13);
  sBorder(40, 8, 2);
  sPositionCur(1, 13);
  sChangeAttr(COLOR(BLK, BLU));
  vRepeat('ß', 38);
  sPositionCur(1, 20);
  vRepeat('Ü', 38);
  sPositionCur(1, 14);
  fBox(0xffff, ' ', 38, 6);
  sChangeAttr(COLOR(CRED, HWHT));
  sPositionCur(2, 14);
  fBox(0xffff, ' ', 36, 6);
  for (i = 0; i < 4; ++i) {
    sPositionCur(2, i + 15);
    asends(alert[i]);
  }
  sPositionCur(5, 22);
  sChangeAttr(COLOR(HWHT, BLK));
  aprintf("Manuever #%d is beginning NOW !", game.number);
  vStatLine("", 0, COLOR(BLK, BLK), 1);
  SelectIO(&CONS);
}

char *GameType(byte type) {
  switch (type) {
    case PUBLIC:
      return ("PUBLIC");
      break;
    case LEAGUE:
      return ("LEAGUE");
      break;
    case FREEFORALL:
      return ("FREE FOR ALL");
      break;
    default:
      return ("unknown mode");
      break;
  }
}

void LOBBY_game1_naughty() {
  char ts[20];

#ifdef EXTENDED_GRAPHICS
  sxToText();
#endif

  SelectIO(&PUB);
  sChangeAttr(COLOR(WHT, BLK));
  sPositionCur(0, 0);
  fBox(0xffff, ' ', 40, 25);

  sPositionCur(0, 0);
  sChangeAttr(COLOR(HBRN, BLU));
  asends(" Score Player ID        Hits OwnP Shot B");
  sPositionCur(0, 1);
  sChangeAttr(COLOR(HWHT, CRED));
  fBox(0xffff, ' ', 40, 1);
  sPositionCur(7, 1);
  asendc(game.redtm1, 10);
  sPositionCur(0, 13);
  sChangeAttr(COLOR(HWHT, GRN));
  fBox(0xffff, ' ', 40, 1);
  sPositionCur(7, 13);
  asendc(game.grntm1, 10);

  vStatLine("Game:", 0, COLOR(WHT, BLU), 1);
  vStatLine("Remaining:", 24, COLOR(WHT, BLU), 0);
  sprintf(ts, "%04d", game.number);
  vStatLine(ts, 6, COLOR(HWHT, BLU), 0);
  if (game.mode1 != PUBLIC)
    vStatLine(GameType(game.mode1), 11, COLOR(HWHT, BLU), 0);
}

void LOBBY_game1_nice() {
  char ts[20];

#ifdef EXTENDED_GRAPHICS
  sxToText();
#endif

  SelectIO(&PUB);
  sChangeAttr(COLOR(WHT, BLK));
  sPositionCur(0, 0);
  fBox(0xffff, ' ', 40, 25);

  sPositionCur(0, 0);
  sChangeAttr(COLOR(HBRN, BLU));

  asends(" Score    Player ID         Hits   Base ");
  sPositionCur(0, 1);
  sChangeAttr(COLOR(HWHT, CRED));
  fBox(0xffff, ' ', 40, 1);
  sPositionCur(10, 1);
  asendc(game.redtm1, 10);
  sPositionCur(0, 13);
  sChangeAttr(COLOR(HWHT, GRN));
  fBox(0xffff, ' ', 40, 1);
  sPositionCur(10, 13);
  asendc(game.grntm1, 10);

  vStatLine("Game:", 0, COLOR(WHT, BLU), 1);
  vStatLine("Remaining:", 24, COLOR(WHT, BLU), 0);
  sprintf(ts, "%04d", game.number);
  vStatLine(ts, 6, COLOR(HWHT, BLU), 0);
  if (game.mode1 != PUBLIC)
    vStatLine(GameType(game.mode1), 11, COLOR(HWHT, BLU), 0);
}

void LOBBY_game1() {
  if ((curconfig.newscr > 1) ||
      ((curconfig.newscr == 1) && (game.mode1 != PUBLIC)))
    LOBBY_game1_naughty();
  else
    LOBBY_game1_nice();
}

int LOBBY_game1_teamud(int color, int naughty) {
  int othercolor, base, obase, vcolor, vbase;
  int hits = 0, gethits = 0, hitown = 0, bases = 0, teamscore = 0;
  char ts[10];

  if (color == RED) {
    othercolor = GREEN;
    base = 0;
    obase = 20;
    vcolor = CRED;
    vbase = 1;
  } else {
    othercolor = RED;
    base = 20;
    obase = 0;
    vcolor = GRN;
    vbase = 13;
  }

  for (i = 1; i <= players[color][0]; ++i) {
    if ((i == 1) &&
        (rankplayer[base + 1]->score > rankplayer[obase + 1]->score) &&
        players[othercolor][0])
      sChangeAttr(COLOR(vcolor, HBLK));
    else
      sChangeAttr(COLOR(vcolor, BLK));
    if (naughty) {
      sPositionCur(7, vbase + i);
      asendc(rankplayer[base + i]->name, 10);
      sPositionCur(0, vbase + i);
      sprintf(ts, "%6d", rankplayer[base + i]->score);
      asends(ts);
      sChangeAttr(COLOR(vcolor, BLK));
      sPositionCur(24, vbase + i);
      aprintf("%4d %4d %4d %c", rankplayer[base + i]->hits,
              rankplayer[base + i]->hitown, rankplayer[base + i]->gethit,
              ((rankplayer[base + i]->baseflag) ? '*' : ' '));
    } else {
      sPositionCur(10, vbase + i);
      asendc(rankplayer[base + i]->name, 10);
      sPositionCur(0, vbase + i);
      sprintf(ts, "%6d", rankplayer[base + i]->score);
      asends(ts);
      sChangeAttr(COLOR(vcolor, BLK));
      sPositionCur(28, vbase + i);
      aprintf("%4d    %c", rankplayer[base + i]->hits,
              ((rankplayer[base + i]->baseflag) ? '*' : ' '));
    }

    gethits += rankplayer[base + i]->gethit;
    hits += rankplayer[base + i]->hits;
    hitown += rankplayer[base + i]->hitown;
    bases += (rankplayer[base + i]->baseflag ? 1 : 0);
    teamscore += rankplayer[base + i]->score;
  }
  sPositionCur(24, vbase);

  sChangeAttr(COLOR(HWHT, vcolor));
  if (naughty)
    aprintf("%4d %4d %4d %c", hits, hitown, gethits,
            ((bases == players[color][0]) ? '*' : ' '));
  else
    aprintf("    %4d    %c", hits, ((bases == players[color][0]) ? '*' : ' '));

  return teamscore;
}

void LOBBY_game1_update(int naughty) {
  int tsr, tsg;
  char ts[10];
  int where;
  SelectIO(&PUB);

  /* the RAD team*/
  tsr = LOBBY_game1_teamud(RED, naughty);
  tsg = LOBBY_game1_teamud(GREEN, naughty);

  if (tsr > tsg)
    sChangeAttr(COLOR(HWHT, HRED));
  else
    sChangeAttr(COLOR(HWHT, CRED));
  sPositionCur(0, 1);
  sprintf(ts, "%6d", tsr);
  asends(ts);

  if (tsg > tsr)
    sChangeAttr(COLOR(HWHT, HGRN));
  else
    sChangeAttr(COLOR(HWHT, GRN));
  sPositionCur(0, 13);
  sprintf(ts, "%6d", tsg);
  asends(ts);

  SelectIO(&CONS);
}

void LOBBY_game2() { /*PUT TWO FIELD SETUP HERE ! ! !*/ }

void setupcga(byte type) {
  switch (type) {
    case BOOT:
      LOBBY_boot();
      break;
    case TWEENTEXT:
      LOBBY_tween();
      break;
    case INSTOKN:
      LOBBY_instokn();
      break;
    case NOXFER:
      LOBBY_noxfer();
      break;
    case ALRTSCRN:
      LOBBY_alert();
      break;
    case GAME1:
      LOBBY_game1();
      break;
    case GAME2:
      LOBBY_game2();
      break;
  }
  /*
CGA_hidecursor();
  */
}

void nl_to_null(char *string) {
  while ((*string != '\n') && (*string != 0)) ++string;
  *string = 0;
}

void clrcga(void) {
  SelectIO(&PUB);
  vClearScreen();
}

#pragma optimize("", off)
void effectsout(char *command, int length) {
  int k, p;
#ifdef DEBUG
  info("Sending to EC:");
  info(command);
#endif
  for (k = 0; k < length; ++k) {
    for (; (slot % 22) != 0;) {
    }
    p = slot;
    for (; (slot == p);) {
    }
    outp(EC, *(command + k));
  }
}
#pragma optimize("", on)

void EtStatus(byte Et1Stat, byte Et2Stat)
/*Stats...
        0       Nothing Sent
        1       Transferring
        2       Awaiting Et start
        3       Awaiting FC start
        4       Unused at the current time
        Et1 is always the RED/ALPHA et
        Et2 is always the GRN/OMEGA et
        5		  FC Start received			  */

{ /* 0123456789012345678901234567890123456789 */
  char *SSS;
  static char *oldSSS = 0;
  char ts[42];
  int i;
  if (Et1Stat == 1) {
    if (curconfig.field == 2) SSS = "READING ALPHA ET CODES";
    if (curconfig.field == 1) SSS = "READING RED ET CODES";
    if (Et2Stat == 4) SSS = "READING ET CODES";
  } else if (Et2Stat == 1) {
    if (curconfig.field == 2) SSS = "READING OMEGA ET CODES";
    if (curconfig.field == 1) SSS = "READING GREEN ET CODES";
    if (Et1Stat == 4) SSS = "READING ET CODES";
  } else if ((Et1Stat == 0) & (Et2Stat == 0))
    SSS = "AWAITING ET CODES";
  else if (Et1Stat == 0) {
    if (Et2Stat == 4)
      SSS = "AWAITING ET CODES";
    else if (curconfig.field == 2)
      SSS = "AWAITING ALPHA ET CODES";
    else if (curconfig.field == 1)
      SSS = "AWAITING RED ET CODES";
  } else if (Et2Stat == 0) {
    if (Et1Stat == 4)
      SSS = "AWAITING ET CODES";
    else if (curconfig.field == 2)
      SSS = "AWAITING OMEGA ET CODES";
    else if (curconfig.field == 1)
      SSS = "AWAITING GREEN ET CODES";
  } else if ((Et1Stat == 2) & (Et2Stat == 2))
    SSS = "AWAITING ET STARTS";
  else if (Et1Stat == 2) {
    if (Et2Stat == 4)
      SSS = "AWAITING ET START";
    else if (curconfig.field == 2)
      SSS = "AWAITING ALPHA ET START";
    else if (curconfig.field == 1)
      SSS = "AWAITING RED ET START";
  } else if (Et2Stat == 2) {
    if (Et1Stat == 4)
      SSS = "AWAITING ET START";
    else if (curconfig.field == 2)
      SSS = "AWAITING OMEGA ET START";
    else if (curconfig.field == 1)
      SSS = "AWAITING GREEN ET START";
  } else if ((Et1Stat == 3) & (Et2Stat == 3))
    SSS = "AWAITING FIELD STARTS";
  else if (Et1Stat == 3) {
    if (Et2Stat == 4)
      SSS = "AWAITING FIELD START";
    else if (curconfig.field == 2)
      SSS = "AWAITING ALPHA FIELD START";
    else if (curconfig.field == 1)
      SSS = "AWAITING RED FIELD START";
  } else if (Et2Stat == 3) {
    if (Et1Stat == 4)
      SSS = "AWAITING FIELD START";
    else if (curconfig.field == 2)
      SSS = "AWAITING OMEGA FIELD START";
    else if (curconfig.field == 1)
      SSS = "AWAITING GREEN FIELD START";
  } else
    SSS = "-*- et error -*-";

  i = strlen(SSS);
  memset(ts, 32, (40 - i) / 2);
  strcpy(ts + ((40 - i) / 2), SSS);
  memset(ts + i + ((40 - i) / 2), 32, 40 - strlen(ts));
  ts[41] = 0;
  if (curconfig.pc) {
    charout(PC, 0xEA);
    HOST_sendsn(PC, ts, 40);
  }
#ifndef EXTENDED_GRAPHICS
  SelectIO(&PUB);
  vStatLine(ts, 0, COLOR(HWHT, BLU), 1);
  SelectIO(&CONS);
#else
  if (SSS != oldSSS) {
    int i;
    sxFontSelect(0);
    sxColorSelect(0, 0);
    for (i = 480; i > 440; --i) sxDrawLine(0, i, 640, i);
    sxColorSelect(15, 0);
    sxPrintText(CEN, FBOT | 1, SSS);
    oldSSS = SSS;
  }
#endif
}

void GetRedTeam(byte teams, byte who) {
  int m, o;
  if (teams == TRUE)
    if (curconfig.field == 1)
      memcpy(game.redtm1, &transfer[0][2], 10);
    else if ((curconfig.field == 2) && (who == ALPHA))
      memcpy(game.redtm1, &transfer[0][2], 10);
    else if ((curconfig.field == 2) && (who == OMEGA))
      memcpy(game.redtm2, &transfer[0][2], 10);

  if (who == BOTH)
    for (m = 0; m < 20; ++m) {
      memcpy(player[m + 1].passport, &transfer[0][(20 * m + 22)], 10);
      memcpy(player[m + 1].name, &transfer[0][(20 * m) + 10 + 22], 10);
      CheckPassport((char *)player[m + 1].passport);
    }
  if (who == ALPHA)
    for (m = 0; m < 10; ++m) {
      memcpy(player[(m * 2) + 1].passport, &transfer[0][(20 * m) + 0 + 22], 10);
      memcpy(player[(m * 2) + 1].name, &transfer[0][(20 * m) + 10 + 22], 10);
      CheckPassport((char *)player[(m * 2) + 1].passport);
    }
  if (who == OMEGA)
    for (m = 0; m < 10; ++m) {
      memcpy(player[(m * 2) + 2].passport, &transfer[0][(20 * m) + 0 + 22], 10);
      memcpy(player[(m * 2) + 2].name, &transfer[0][(20 * m) + 10 + 22], 10);
      CheckPassport((char *)player[(m * 2) + 2].passport);
    }
}

void GetGrnTeam(byte teams, byte who) {
  int m, o;
  if (teams == TRUE)
    if (curconfig.field == 1)
      memcpy(game.grntm1, &transfer[1][12], 10);
    else if ((curconfig.field == 2) & (who == ALPHA))
      memcpy(game.grntm1, &transfer[1][12], 10);
    else if ((curconfig.field == 2) & (who == OMEGA))
      memcpy(game.grntm2, &transfer[1][12], 10);
  if (who == BOTH)
    for (m = 0; m < 20; ++m) {
      memcpy(player[m + 21].passport, &transfer[1][(20 * m + 22)], 10);
      memcpy(player[m + 21].name, &transfer[1][(20 * m) + 10 + 22], 10);
      CheckPassport((char *)player[m + 21].passport);
    }
  if (who == GREEN)
    for (m = 0; m < 20; ++m) {
      memcpy(player[m + 21].passport, &transfer[0][(20 * m) + 22], 10);
      memcpy(player[m + 21].name, &transfer[0][(20 * m) + 10 + 22], 10);
      CheckPassport((char *)player[m + 21].passport);
    }
  if (who == ALPHA)
    for (m = 0; m < 10; ++m) {
      memcpy(player[(m * 2) + 21].passport, &transfer[1][(20 * m) + 22], 10);
      memcpy(player[(m * 2) + 21].name, &transfer[1][(20 * m) + 10 + 22], 10);
      CheckPassport((char *)player[(m * 2) + 21].passport);
    }
  if (who == OMEGA)
    for (m = 0; m < 10; ++m) {
      memcpy(player[(m * 2) + 22].passport, &transfer[1][(20 * m) + 22], 10);
      memcpy(player[(m * 2) + 22].name, &transfer[1][(20 * m) + 10 + 22], 10);
      CheckPassport((char *)player[(m * 2) + 22].passport);
    }
}

void SelectTrack(byte track) {
  if (curconfig.cdsel == RANDOM)
    gametrack = track_lut[(byte)(getrandom(0, 20))];
  else
    gametrack = (byte)((game.number) % 5);
  if (gametrack == 0) gametrack = 5;
  CD_track(gametrack);
  if (curconfig.ec == TRUE) settrack();
  CD_track(gametrack);
}
