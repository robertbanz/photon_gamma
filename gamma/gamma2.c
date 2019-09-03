/*************
 gamma2.c
 *************/

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

void setupcga(byte type) {
  int x, y;
  FILE *fp;
  char filename[20];
  char ts[90];
  char tempstring[50];
  char *SSS;
  CGA_hidecursor();
  if (type == BOOT) {
    vPage(0);
    vPosCur(0, 0);
    vChangeAttr(COLOR(BLK, WHT));
    vStatLine("", 0, COLOR(BLK, WHT), 1);
    vBox(0xffff, ' ', 40, 24);
    vChangeAttr(COLOR(HWHT, BLU));
    vPosCur(0, 0);
    vBox(0xffff, ' ', 40, 4);
    vPosCur(5, 1);
    v_printf("THIS IS GAMMA SYSTEMS COMPUTER");
    vPosCur(10, 2);
    v_printf("%s.%s.%s.%s.%s", STORENUM, SITENUM, OPTNUM, LICNUM, REVNUM);
    vChangeAttr(COLOR(BLK, WHT));
    vPosCur(3, 5);
    v_printf("PHOTON system software,");
    vPosCur(8, 6);
    v_printf("(c) 1991,1992 Robert Banz");
    vPosCur(8, 7);
    v_printf("All Rights Reserved");
    vPosCur(3, 9);
    v_printf("Some screen displays and other stuff");
    vPosCur(8, 10);
    v_printf("(c) 1991,1992 Chris Fanning & John Shappert");
    vPosCur(3, 12);
    v_printf("Configuring System ...");
    vPosCur(8, 15);
    v_printf("Core:083092 w/(rel BETA 062187)");
    vPosCur(8, 16);
    v_printf("  PC:060692 w/(rel 0.6  060692)");
    vPosCur(8, 17);
    v_printf("  DC:080192 w/(rel 1.5b 080192)");
    vPosCur(8, 18);
    v_printf("  ET:011092 w/(rel BETA 062187)");
    vPage(1);
  } else if (type == TWEENTEXT) {
    vPage(0);
    vChangeAttr(COLOR(BLK, WHT));
    vPosCur(0, 0);
    vBox(0xffff, ' ', 40, 24);
    vStatLine("", 0, COLOR(HWHT, BLU), 1);
    vPosCur(0, 0);
    vChangeAttr(COLOR(HWHT, BLU));
    vBox(0xffff, ' ', 40, 4);
    vPosCur(0, 16);
    vBox(0xffff, ' ', 40, 1);
    vChangeAttr(COLOR(BLU, WHT));
    vPosCur(0, 23);
    vBox(0xffff, 220, 40, 1);
    vChangeAttr(COLOR(HWHT, BLU));
    vPosCur(5, 1);
    v_printf("THIS IS GAMMA SYSTEMS COMPUTER");
    vPosCur(10, 2);
    v_printf("%s.%s.%s.%s.%s", STORENUM, SITENUM, OPTNUM, LICNUM, REVNUM);
    vChangeAttr(COLOR(BLU, WHT));
    vPosCur(5, 18);
    v_printf("ET TRANSPORT LINK NOW OPEN:");
    vPosCur(5, 20);
    v_printf("EARTH MAY NOW BEGIN TRANSPORTING");
    vPosCur(5, 21);
    v_printf("WARRIORS TO PLANET PHOTON.");
    ++tnum;
    sprintf(filename, "%smess.%d", curconfig.mess_path, tnum);
    if ((fp = fopen(filename, "r")) != NULL) {
      fgets(tempstring, 40, fp);
      getc(fp);
      getc(fp);
      getc(fp);
      for (y = 0; y < 10; ++y) { /*add 5 to y when printing*/
        fgets(textinfo[y], 40, fp);
        nl_to_null(textinfo[y]);
        fgetc(fp);
        vPosCur(0, y + 5);
        vChangeAttr(COLOR(BLK, WHT));
        v_sends(textinfo[y]);
      }
    }
    fclose(fp);
    if (tnum == 10) tnum = 0;
    vPage(1);
  } else if (type == INSTOKN) {
    vPage(0);
    vPosCur(0, 0);
    vChangeAttr(COLOR(BLK, WHT));
    vBox(0xffff, ' ', 40, 24);
    vStatLine("", 0, COLOR(BLK, WHT), 1);
    vPosCur(0, 0);
    vChangeAttr(COLOR(HWHT, BLU));
    vBox(0xffff, ' ', 40, 4);
    vPosCur(0, 18);
    vChangeAttr(COLOR(BLU, WHT));
    vBox(0xffff, 220, 40, 1);
    vPosCur(0, 23);
    vBox(0xffff, 220, 40, 1);
    vPosCur(0, 15);
    vBox(0xffff, 220, 40, 1);
    vPosCur(8, 17);
    vChangeAttr(COLOR(CRED, HWHT));
    if (curconfig.ps == TRUE)
      v_printf("INSERT YOUR TOKENS NOW!!!");
    else
      v_printf("     CONDITION YELLOW    ");
    vChangeAttr(COLOR(HWHT, BLU));
    vPosCur(5, 1);
    v_printf("THIS IS GAMMA SYSTEMS COMPUTER");
    vPosCur(10, 2);
    v_printf("%s.%s.%s.%s.%s", STORENUM, SITENUM, OPTNUM, LICNUM, REVNUM);
    for (y = 0; y < 10; ++y) {
      vPosCur(0, y + 5);
      vChangeAttr(COLOR(BLK, WHT));
      v_sends(textinfo[y]);
    }
    DispGameModes();
    vPage(1);
  }

  else if (type == NOXFER) {
    vPage(0);
    vChangeAttr(COLOR(HWHT, BLU));
    vPosCur(0, 23);
    vBox(0xffff, ' ', 40, 1);
    vStatLine("", 0, COLOR(HWHT, BLU), 1);
    vPage(1);
  } else if (type == ALRTSCRN) {
    vPage(0);
    vPosCur(0, 0);
    vChangeAttr(COLOR(BLK, WHT));
    vBox(0xffff, ' ', 40, 24);
    vPosCur(0, 0);
    vChangeAttr(COLOR(HWHT, BLU));
    vBox(0xffff, ' ', 40, 4);
    vPosCur(5, 1);
    v_printf("THIS IS GAMMA SYSTEMS COMPUTER");
    vPosCur(10, 2);
    v_printf("%s.%s.%s.%s.%s", STORENUM, SITENUM, OPTNUM, LICNUM, REVNUM);
    vChangeAttr(COLOR(CRED, HWHT));
    if (curconfig.ps == TRUE) {
      vPosCur(3, 17);
      v_printf("EARTHLINGS MAN YOUR PHASOR STATIONS");
    } else {
      vPosCur(3, 17);
      v_printf("         CONDITION RED");
    }
    vPosCur(0, 18);
    vChangeAttr(COLOR(BLU, WHT));
    vBox(0xffff, 220, 40, 1);
    vPosCur(0, 23);
    vBox(0xffff, 220, 40, 1);
    vChangeAttr(COLOR(BLK, WHT));
    sprintf(ts, "%2d", game.number);
    vStatLine("   MANEUVER #   START IN ----->T-   sec", 0, COLOR(HWHT, BLU),
              1);
    vStatLine(ts, 13, COLOR(HWHT, BLU), 0);

    vChangeAttr(COLOR(CRED, HWHT));
    vPosCur(8, 6);
    v_printf(" лл  л    лллл лллн ллллл");
    vPosCur(8, 7);
    v_printf("онон л    л    л ол   л");
    vPosCur(8, 8);
    v_printf("л  л л    л    л  л   л");
    vPosCur(8, 9);
    v_printf("л  л л    л    л ол   л");
    vPosCur(8, 10);
    v_printf("л  л л    ллл  лллн   л");
    vPosCur(8, 11);
    v_printf("лллл л    л    л он   л");
    vPosCur(8, 12);
    v_printf("л  л л    л    л  л   л");
    vPosCur(8, 13);
    v_printf("л  л л    л    л  л   л");
    vPosCur(8, 14);
    v_printf("л  л лллл лллл л  л   л");
    DispGameModes();
    vPage(1);
  } else if (type == GAME1) {
    if ((curconfig.newscr > 1) ||
        ((curconfig.newscr == 1) && (game.mode1 != PUBLIC))) {
      vPage(0);
      vChangeAttr(COLOR(WHT, BLK));
      vClearScreen();
      sprintf(ts, " Maneuver %04d      Time Remaining", game.number);
      vStatLine(ts, 0, COLOR(HWHT, BLU), 1);
      vChangeAttr(COLOR(HWHT, BLU));
      vPosCur(0, 0);
      v_sends(" Photon ID        Hits Down OwnP  Score ");
      vPosCur(0, 23);
      switch (game.mode1) {
        case LEAGUE:
          SSS = "LEAGUE";
          break;
        case FREEFORALL:
          SSS = "FREE FOR ALL";
          break;
        case PUBLIC:
          SSS = "PUBLIC";
          break;
        default:
          SSS = "Unknown";
          break;
      }

      v_printf(" %-16s    %2d:%02d - %2d-%02d-%04d ", SSS, curtime.hour,
               curtime.minute, curdate.month, curdate.day, curdate.year);
      vPosCur(0, 1);
      vChangeAttr(COLOR(HWHT, CRED));
      vLineEdit(9);
      v_printf(" %10s", game.redtm1);
      vPosCur(0, 12);
      vChangeAttr(COLOR(HWHT, GRN));
      vLineEdit(9);
      v_printf(" %10s", game.grntm1);
      vPage(1);
    } else {
      vPage(0);
      vChangeAttr(COLOR(WHT, BLK));
      vClearScreen();
      sprintf(ts, " Maneuver %04d      Time Remaining", game.number);
      vStatLine(ts, 0, COLOR(HWHT, BLU), 1);
      vChangeAttr(COLOR(HWHT, BLU));
      vPosCur(0, 0);
      v_sends("    Photon ID            Hits    Score  ");
      vPosCur(0, 23);
      switch (game.mode1) {
        case LEAGUE:
          SSS = "LEAGUE";
          break;
        case FREEFORALL:
          SSS = "FREE FOR ALL";
          break;
        case PUBLIC:
          SSS = "PUBLIC";
          break;
        default:
          SSS = "Unknown";
          break;
      }

      v_printf(" %-16s    %2d:%02d - %2d-%02d-%04d ", SSS, curtime.hour,
               curtime.minute, curdate.month, curdate.day, curdate.year);
      vPosCur(0, 1);
      vChangeAttr(COLOR(HWHT, CRED));
      vLineEdit(9);
      v_printf("    %10s", game.redtm1);
      vPosCur(0, 12);
      vChangeAttr(COLOR(HWHT, GRN));
      vLineEdit(9);
      v_printf("    %10s", game.grntm1);
      vPage(1);
    }
  }

  /***************************** THIS IS WHERE WE'RE GONNA WORK ******/
  else if (type == GAME2) {
    for (y = 0; y < 25; ++y)
      for (x = 0; x < 40; ++x) cga1[0][y][x] = BLUEA | GRNA | REDA | INTA;
    for (y = 2; y <= 10; ++y)
      for (x = 1; x <= 18; ++x) cga1[0][y][x] = REDA | INTA;
    for (y = 2; y <= 10; ++y)
      for (x = 20; x <= 37; ++x) cga1[0][y][x] = GRNA | INTA;
    for (y = 14; y <= 23; ++y)
      for (x = 1; x <= 18; ++x) cga1[0][y][x] = REDA | INTA;
    for (y = 14; y <= 23; ++y)
      for (x = 20; x <= 37; ++x) cga1[0][y][x] = GRNA | INTA;

    for (y = 0; y < 25; ++y) {
      cga1[0][y][38] = BLUEAb | GRNA | BLUEA | REDA;
      cga1[0][y][39] = BLUEAb | GRNA | BLUEA | REDA;
    }
    for (x = 0; x < 40; ++x) cga1[0][12][x] = BLUEAb | GRNA | BLUEA | REDA;
    cga1[0][12][10] = BLUEAb | GRNA | BLUEA | REDA | INTA;
    cga1[0][12][11] = BLUEAb | GRNA | BLUEA | REDA | INTA;
    cga1[0][12][32] = BLUEAb | GRNA | BLUEA | REDA | INTA;
    cga1[0][12][33] = BLUEAb | GRNA | BLUEA | REDA | INTA;
    cga1[0][12][34] = BLUEAb | GRNA | BLUEA | REDA | INTA;
    sprintf(&cga1[1][12][1], "MANEUVER XX     TIME REMAINING XXXsec");
    for (x = 0; x < 38; ++x) {
      cga1[0][11][x] = GRNA | BLUEA | REDA | INTA;
      cga1[0][13][x] = GRNA | BLUEA | REDA | INTA;
      cga1[0][24][x] = GRNA | BLUEA | REDA | INTA;
    }
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
  if (Et1Stat == 1) {
    if (curconfig.field == 2) SSS = "          READING ALPHA ET CODES        ";
    if (curconfig.field == 1) SSS = "           READING RED ET CODES         ";
    if (Et2Stat == 4) SSS = "             READING ET CODES           ";
  } else if (Et2Stat == 1) {
    if (curconfig.field == 2) SSS = "          READING OMEGA ET CODES        ";
    if (curconfig.field == 1) SSS = "          READING GREEN ET CODES        ";
    if (Et1Stat == 4) SSS = "             READING ET CODES           ";
  } else if ((Et1Stat == 0) & (Et2Stat == 0))
    SSS = "            AWAITING ET CODES           ";
  else if (Et1Stat == 0) {
    if (Et2Stat == 4)
      SSS = "            AWAITING ET CODES           ";
    else if (curconfig.field == 2)
      SSS = "         AWAITING ALPHA ET CODES        ";
    else if (curconfig.field == 1)
      SSS = "          AWAITING RED ET CODES         ";
  } else if (Et2Stat == 0) {
    if (Et1Stat == 4)
      SSS = "            AWAITING ET CODES           ";
    else if (curconfig.field == 2)
      SSS = "         AWAITING OMEGA ET CODES        ";
    else if (curconfig.field == 1)
      SSS = "         AWAITING GREEN ET CODES        ";
  } else if ((Et1Stat == 2) & (Et2Stat == 2))
    SSS = "           AWAITING ET STARTS           ";
  else if (Et1Stat == 2) {
    if (Et2Stat == 4)
      SSS = "            AWAITING ET START           ";
    else if (curconfig.field == 2)
      SSS = "         AWAITING ALPHA ET START        ";
    else if (curconfig.field == 1)
      SSS = "          AWAITING RED ET START         ";
  } else if (Et2Stat == 2) {
    if (Et1Stat == 4)
      SSS = "            AWAITING ET START           ";
    else if (curconfig.field == 2)
      SSS = "         AWAITING OMEGA ET START        ";
    else if (curconfig.field == 1)
      SSS = "         AWAITING GREEN ET START        ";
  } else if ((Et1Stat == 3) & (Et2Stat == 3))
    SSS = "          AWAITING FIELD STARTS         ";
  else if (Et1Stat == 3) {
    if (Et2Stat == 4)
      SSS = "           AWAITING FIELD START         ";
    else if (curconfig.field == 2)
      SSS = "         AWAITING ALPHA FIELD START 	   ";
    else if (curconfig.field == 1)
      SSS = "         AWAITING RED FIELD START       ";
  } else if (Et2Stat == 3) {
    if (Et1Stat == 4)
      SSS = "           AWAITING FIELD START         ";
    else if (curconfig.field == 2)
      SSS = "        AWAITING OMEGA FIELD START      ";
    else if (curconfig.field == 1)
      SSS = "        AWAITING GREEN FIELD START       ";
  } else
    SSS = "";
  vPage(0);
  vStatLine(SSS, 0, COLOR(HWHT, BLU), 1);
  vPage(1);
}

void GetRedTeam(byte teams, byte who) {
  int m, o;
  if (teams == TRUE)
    for (m = 0; m < 10; ++m)
      if (curconfig.field == 1)
        game.redtm1[m] = transfer[0][2 + m];
      else if ((curconfig.field == 2) && (who == ALPHA))
        game.redtm1[m] = transfer[0][2 + m];
      else if ((curconfig.field == 2) && (who == OMEGA))
        game.redtm2[m] = transfer[0][2 + m];
  if (who == BOTH)
    for (m = 0; m < 20; ++m)
      for (o = 0; o < 10; ++o) {
        player[m + 1].passport[o] = transfer[0][(20 * m) + o + 22];
        player[m + 1].name[o] = transfer[0][(20 * m) + 10 + o + 22];
        CheckPassport(&player[m + 1].passport);
      }
  if (who == ALPHA)
    for (m = 0; m < 10; ++m)
      for (o = 0; o < 10; ++o) {
        player[(m * 2) + 1].passport[o] = transfer[0][(20 * m) + o + 22];
        player[(m * 2) + 1].name[o] = transfer[0][(20 * m) + 10 + o + 22];
        CheckPassport(&player[(m * 2) + 1].passport);
      }
  if (who == OMEGA)
    for (m = 0; m < 10; ++m)
      for (o = 0; o < 10; ++o) {
        player[(m * 2) + 2].passport[o] = transfer[0][(20 * m) + o + 22];
        player[(m * 2) + 2].name[o] = transfer[0][(20 * m) + 10 + o + 22];
        CheckPassport(&player[(m * 2) + 2].passport);
      }
}

void GetGrnTeam(byte teams, byte who) {
  int m, o;
  if (teams == TRUE)
    for (m = 0; m < 10; ++m)
      if (curconfig.field == 1)
        game.grntm1[m] = transfer[1][12 + m];
      else if ((curconfig.field == 2) & (who == ALPHA))
        game.grntm1[m] = transfer[1][12 + m];
      else if ((curconfig.field == 2) & (who == OMEGA))
        game.grntm2[m] = transfer[1][12 + m];
  if (who == BOTH)
    for (m = 0; m < 20; ++m)
      for (o = 0; o < 10; ++o) {
        player[m + 21].passport[o] = transfer[1][(20 * m) + o + 22];
        player[m + 21].name[o] = transfer[1][(20 * m) + 10 + o + 22];
        CheckPassport(&player[m + 21].passport);
      }
  if (who == GREEN)
    for (m = 0; m < 20; ++m)
      for (o = 0; o < 10; ++o) {
        player[m + 21].passport[o] = transfer[0][(20 * m) + o + 22];
        player[m + 21].name[o] = transfer[0][(20 * m) + 10 + o + 22];
        CheckPassport(&player[m + 21].passport);
      }
  if (who == ALPHA)
    for (m = 0; m < 10; ++m)
      for (o = 0; o < 10; ++o) {
        player[(m * 2) + 21].passport[o] = transfer[1][(20 * m) + o + 22];
        player[(m * 2) + 21].name[o] = transfer[1][(20 * m) + 10 + o + 22];
        CheckPassport(&player[(m * 2) + 21].passport);
      }
  if (who == OMEGA)
    for (m = 0; m < 10; ++m)
      for (o = 0; o < 10; ++o) {
        player[(m * 2) + 22].passport[o] = transfer[1][(20 * m) + o + 22];
        player[(m * 2) + 22].name[o] = transfer[1][(20 * m) + 10 + o + 22];
        CheckPassport(player[(m * 2) + 22].passport);
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
