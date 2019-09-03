/*************
 gamma2.c
 *************/
#define SYSTEM_PROGRAM
#undef __TOPLEV
#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include "gamma.h"
#include "video.h"
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
  switch (gametrack) {
    case 1:
      effectsout("RU1", 3);
      break;
    case 2:
      effectsout("RU2", 3);
      break;
    case 3:
      effectsout("RU3", 3);
      break;
    case 4:
      effectsout("RU4", 3);
      break;
    case 5:
      effectsout("RU5", 3);
      break;
    case 6:
      effectsout("RU6", 3);
      break;
    case 7:
      effectsout("RU7", 3);
      break;
    case 8:
      effectsout("RU8", 3);
      break;
  }
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

void LOBBY_boot(void) {
  char ts[80];
  vPage(0);
  vPosCur(0, 0);
  vChangeAttr(COLOR(BLK, WHT));
  vStatLine("", 0, COLOR(BLK, WHT), 1);
  vBox(0xffff, ' ', 40, 24);

  vChangeAttr(COLOR(HWHT, BLU));
  vBox(0xffff, ' ', 40, 4);

  vPosCur(1, 0);
  v_sends("Stimpy, sometimes your wealth of");
  vPosCur(1, 1);
  v_sends("ignorance astounds me...this MUST");
  vPosCur(1, 2);
  sprintf(ts, "be the Gamma Systems Computer V%s", SYSTEMVERSION);
  v_sends(ts);
  vChangeAttr(COLOR(BLK, WHT));
  vPosCur(1, 6);
  v_sends("Photon Systems Software");
  vPosCur(1, 8);
  v_sends("(c) 1991,92,93 Robert Banz and");
  vPosCur(10, 9);
  v_sends("Chris Fanning.");
  vPosCur(10, 10);
  v_sends("All Rights Reserved");
  vPosCur(1, 12);
  v_sends("Licensed to Quantum Entertainment Inc.");
  vPosCur(1, 14);
  v_sends(__DATE__);
  vPosCur(1, 16);
  v_sends("The system is coming up, please wait.");
  vPage(1);
}

void PrintMessage(int dummy) {
  int x, y;
  FILE *fp;
  char filename[20];
  char ts[90];
  char tempstring[50];

  sprintf(filename, "%smess.%d", curconfig.mess_path, tnum);
  if ((fp = fopen(filename, "r")) != NULL) {
    fgets(tempstring, 40, fp);
    getc(fp);
    getc(fp);
    getc(fp);
    vChangeAttr(COLOR(BLK, WHT));
    vPosCur(0, 3);
    vBox(0xffff, ' ', 40, 10);
    for (y = 0; y < 10; ++y) {
      fgets(textinfo[y], 40, fp);
      nl_to_null(textinfo[y]);
      fgetc(fp);
      vPosCur(0, (byte)(y + 3));
      v_sends(textinfo[y]);
    }
  }
  fclose(fp);
  if (tnum == 10) tnum = 0;
  vPage(1);
}

char *TitleString = "Photon Systems Computer ALPHA_01";

void LOBBY_tween() {
  int x, y;
  char ts[90];
  char *SSS;

  vPage(0);
  vChangeAttr(COLOR(BLK, WHT));
  vPosCur(0, 0);
  vBox(0xffff, ' ', 40, 24);
  vChangeAttr(COLOR(HWHT, BLU));
  vBox(0xffff, ' ', strlen(TitleString) + 2, 2);
  v_sends(TitleString);
  vChangeAttr(COLOR(BLK, WHT));
  vPosCur(0, 2);
  vRepeat('Í', strlen(TitleString) + 2);
  for (i = 0; i < 2; ++i) {
    vPosCur(strlen(TitleString) + 2, i);
    vPutch('³');
  }
  vPosCur(strlen(TitleString) + 2, 2);
  vPutch('¾');
  vPosCur(0, 13);
  vBorder(40, 8, 2);
  vPosCur(1, 13);
  vChangeAttr(COLOR(WHT, BLU));
  vRepeat('ß', 38);
  vPosCur(1, 20);
  vRepeat('Ü', 38);
  vPosCur(1, 14);
  vBox(0xffff, ' ', 38, 6);
  vChangeAttr(COLOR(BLK, WHT));
  vPosCur(1, 21);
  v_sends("Photon Time");
  vPosCur(26, 21);
  v_sends("Next Manuever");
  vPosCur(30, 22);
  v_printf("%04d", game.number);
  vChangeAttr(COLOR(BLK, WHT));
  vPosCur(4, 22);
  sprintf(ts, "%2d:%02d", (int)curtime.hour, (int)curtime.minute);
  v_sends(ts);
  PrintMessage(++tnum);
  vPage(0);
  /*print the high scores in the high score zone*/
  vChangeAttr(COLOR(HBRN, BLU));
  vPosCur(2, 14);
  v_sends("High score of the week:");
  vPosCur(2, 17);
  v_sends("Highest hits of the week:");
  vChangeAttr(COLOR(HWHT, BLU));
  vPosCur(7, 15);
  if (weekly.highscore.passport[0]) {
    v_sendsn(weekly.highscore.name, 10);
    vPosCur(23, 15);
    sprintf(ts, "%6d", weekly.highscore.score);
    v_sends(ts);
  } else
    v_sends("None yet!  It could be YOU!");
  vPosCur(7, 18);
  if (weekly.highhits.passport[0]) {
    v_sendsn(weekly.highscore.name, 10);
    vPosCur(23, 18);
    v_printf("%3d", weekly.highhits.hits);
  } else
    v_sends("None yet!  It could be YOU!");
  vPage(1);
}

void LOBBY_instokn() {
  int y;
  char ts[80];

  vPage(0);
  vChangeAttr(COLOR(BLK, WHT));
  vPosCur(0, 0);
  vBox(0xffff, ' ', 40, 24);
  vChangeAttr(COLOR(HWHT, BLU));
  vBox(0xffff, ' ', strlen(TitleString) + 2, 2);
  v_sends(TitleString);
  vChangeAttr(COLOR(BLK, WHT));
  vPosCur(0, 2);
  vRepeat('Í', strlen(TitleString) + 2);
  for (i = 0; i < 2; ++i) {
    vPosCur(strlen(TitleString) + 2, i);
    vPutch('³');
  }
  vPosCur(strlen(TitleString) + 2, 2);
  vPutch('¾');
  vPosCur(0, 13);
  vBorder(40, 8, 2);
  vPosCur(1, 13);
  vChangeAttr(COLOR(WHT, BLU));
  vRepeat('ß', 38);
  vPosCur(1, 20);
  vRepeat('Ü', 38);
  vPosCur(1, 14);
  vBox(0xffff, ' ', 38, 6);
  vChangeAttr(COLOR(BLK, WHT));
  vPosCur(1, 21);
  v_sends("Photon Time");
  vPosCur(26, 21);
  v_sends("Next Manuever");
  vPosCur(30, 22);
  v_printf("%04d", game.number);
  vChangeAttr(COLOR(BLK, WHT));
  vPosCur(4, 22);
  sprintf(ts, "%2d:%02d", (int)curtime.hour, (int)curtime.minute);
  v_sends(ts);
  PrintMessage(((tnum == 0) ? tnum++ : tnum));
  vPage(0);
  vPosCur(2, 14);
  vChangeAttr(COLOR(BLK, WHT));
  vBox(0xffff, ' ', 36, 6);
  vChangeAttr(COLOR(CRED, HWHT));
  vPosCur(3, 14);
  v_sends("*ATTENTION:*");
  vChangeAttr(COLOR(BLK, WHT));
  vPosCur(16, 14);
  v_sends("Maneuver in final");
  vPosCur(4, 15);
  v_sends("phase of staging.");
  vPosCur(4, 18);
  v_sends("Next maneuver please report to");
  vPosCur(4, 19);
  v_sends("staging area now !");

  /*DispGameModes();*/
  vPage(1);
}

void LOBBY_noxfer() {
  vPage(0);
  vChangeAttr(COLOR(HWHT, BLU));
  vStatLine("", 0, COLOR(HWHT, BLU), 1);
  vPage(1);
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
  vPosCur(x, y);
  vBox(0xffff, ' ', 6, 7);

  if ((digit >= 0) && (digit < 10)) {
    for (i = 0; i < 7; ++i) {
      if (data[digit][i]) {
        vPosCur(x + s[i][0], y + s[i][1]);
        vBox(0xffff, 219, s[i][2], s[i][3]);
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

  vPage(0);
  vChangeAttr(COLOR(HWHT, BLK));
  vPosCur(0, 0);
  vBox(0xffff, ' ', 40, 24);
  vChangeAttr(COLOR(HWHT, BLU));
  vBox(0xffff, ' ', strlen(TitleString) + 2, 2);
  v_sends(TitleString);
  vChangeAttr(COLOR(HWHT, BLK));
  vPosCur(0, 2);
  vRepeat('Í', strlen(TitleString) + 2);
  for (i = 0; i < 2; ++i) {
    vPosCur(strlen(TitleString) + 2, i);
    vPutch('³');
  }
  vPosCur(strlen(TitleString) + 2, 2);
  vPutch('¾');
  vChangeAttr(COLOR(BLU, BLK));
  vPosCur(7, 6);
  vPutch('ß');
  vPosCur(8, 6);
  vPutch('Û');
  vPosCur(9, 6);
  vPutch('ß');
  vPosCur(8, 7);
  vPutch('Û');
  vPosCur(8, 8);
  vPutch('ß');
  vPosCur(11, 7);
  v_sends("ßß");
  vChangeAttr(COLOR(HWHT, BLK));
  vPosCur(0, 13);
  vBorder(40, 8, 2);
  vPosCur(1, 13);
  vChangeAttr(COLOR(BLK, BLU));
  vRepeat('ß', 38);
  vPosCur(1, 20);
  vRepeat('Ü', 38);
  vPosCur(1, 14);
  vBox(0xffff, ' ', 38, 6);
  vChangeAttr(COLOR(CRED, HWHT));
  vPosCur(2, 14);
  vBox(0xffff, ' ', 36, 6);
  for (i = 0; i < 4; ++i) {
    vPosCur(2, i + 15);
    v_sends(alert[i]);
  }
  vPosCur(5, 22);
  vChangeAttr(COLOR(HWHT, BLK));
  v_printf("Manuever #%d is beginning NOW !", game.number);
  vStatLine("", 0, COLOR(BLK, BLK), 1);
  vPage(1);
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

  vPage(0);
  vChangeAttr(COLOR(WHT, BLK));
  vPosCur(0, 0);
  vBox(0xffff, ' ', 40, 25);

  vPosCur(0, 0);
  vChangeAttr(COLOR(HBRN, BLU));
  v_sends(" Score Player ID        Hits OwnP Shot B");
  vPosCur(0, 1);
  vChangeAttr(COLOR(HWHT, CRED));
  vBox(0xffff, ' ', 40, 1);
  vPosCur(7, 1);
  v_sendsn(game.redtm1, 10);
  vPosCur(0, 13);
  vChangeAttr(COLOR(HWHT, GRN));
  vBox(0xffff, ' ', 40, 1);
  vPosCur(7, 13);
  v_sendsn(game.grntm1, 10);

  vStatLine("Game:", 0, COLOR(WHT, BLU), 1);
  vStatLine("Remaining:", 24, COLOR(WHT, BLU), 0);
  sprintf(ts, "%04d", game.number);
  vStatLine(ts, 6, COLOR(HWHT, BLU), 0);
  if (game.mode1 != PUBLIC)
    vStatLine(GameType(game.mode1), 11, COLOR(HWHT, BLU), 0);
}

void LOBBY_game1_nice() {
  char ts[20];

  vPage(0);
  vChangeAttr(COLOR(WHT, BLK));
  vPosCur(0, 0);
  vBox(0xffff, ' ', 40, 25);

  vPosCur(0, 0);
  vChangeAttr(COLOR(HBRN, BLU));
  /*      0123456789012345678901234567890123456789*/
  v_sends(" Score    Player ID         Hits   Base ");
  vPosCur(0, 1);
  vChangeAttr(COLOR(HWHT, CRED));
  vBox(0xffff, ' ', 40, 1);
  vPosCur(10, 1);
  v_sendsn(game.redtm1, 10);
  vPosCur(0, 13);
  vChangeAttr(COLOR(HWHT, GRN));
  vBox(0xffff, ' ', 40, 1);
  vPosCur(10, 13);
  v_sendsn(game.grntm1, 10);

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
      vChangeAttr(COLOR(vcolor, HBLK));
    else
      vChangeAttr(COLOR(vcolor, BLK));
    if (naughty) {
      vPosCur(7, vbase + i);
      v_sendsn(rankplayer[base + i]->name, 10);
      vPosCur(0, vbase + i);
      sprintf(ts, "%6d", rankplayer[base + i]->score);
      v_sends(ts);
      vChangeAttr(COLOR(vcolor, BLK));
      vPosCur(24, vbase + i);
      v_printf("%4d %4d %4d %c", rankplayer[base + i]->hits,
               rankplayer[base + i]->hitown, rankplayer[base + i]->gethit,
               ((rankplayer[base + i]->baseflag) ? '*' : ' '));
    } else {
      vPosCur(10, vbase + i);
      v_sendsn(rankplayer[base + i]->name, 10);
      vPosCur(0, vbase + i);
      sprintf(ts, "%6d", rankplayer[base + i]->score);
      v_sends(ts);
      vChangeAttr(COLOR(vcolor, BLK));
      vPosCur(28, vbase + i);
      v_printf("%4d    %c", rankplayer[base + i]->hits,
               ((rankplayer[base + i]->baseflag) ? '*' : ' '));
    }

    gethits += rankplayer[base + i]->gethit;
    hits += rankplayer[base + i]->hits;
    hitown += rankplayer[base + i]->hitown;
    bases += (rankplayer[base + i]->baseflag ? 1 : 0);
    teamscore += rankplayer[base + i]->score;
  }
  vPosCur(24, vbase);

  vChangeAttr(COLOR(HWHT, vcolor));
  if (naughty)
    v_printf("%4d %4d %4d %c", hits, hitown, gethits,
             ((bases == players[color][0]) ? '*' : ' '));
  else
    v_printf("    %4d    %c", hits, ((bases == players[color][0]) ? '*' : ' '));

  return teamscore;
}

LOBBY_game1_update(int naughty) {
  int tsr, tsg;
  char ts[10];
  int where;
  vPage(0);

  /* the RAD team*/
  tsr = LOBBY_game1_teamud(RED, naughty);
  tsg = LOBBY_game1_teamud(GREEN, naughty);

  if (tsr > tsg)
    vChangeAttr(COLOR(HWHT, HRED));
  else
    vChangeAttr(COLOR(HWHT, CRED));
  vPosCur(0, 1);
  sprintf(ts, "%6d", tsr);
  v_sends(ts);

  if (tsg > tsr)
    vChangeAttr(COLOR(HWHT, HGRN));
  else
    vChangeAttr(COLOR(HWHT, GRN));
  vPosCur(0, 13);
  sprintf(ts, "%6d", tsg);
  v_sends(ts);

  vPage(1);
}

void LOBBY_game2() { /*PUT TWO FIELD SETUP HERE ! ! !*/ }

void setupcga(byte type) {
  CGA_hidecursor();
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
}

void nl_to_null(char *string) {
  while ((*string != '\n') && (*string != 0)) ++string;
  *string = 0;
}

void clrcga(void) {
  vPage(0);
  vClearScreen();
}

#pragma optimize("", off)
void effectsout(char *command, int length) {
  int k, p;

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

void CGA_hidecursor(void) {
  outp(0x3d4, 14);
  outp(0x3d5, 0x1f);
  outp(0x3d4, 15);
  outp(0x3d5, 0x41);
}

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

  vPage(0);
  vStatLine(ts, 0, COLOR(HWHT, BLU), 1);
  vPage(1);
}

/*this stuff sucks.  rewrite it or I'll kill me*/

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
