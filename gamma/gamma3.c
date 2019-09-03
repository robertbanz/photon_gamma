/*************
 gamma3.c
 *************/

#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <sys\timeb.h>
#include <sys\types.h>
#include <time.h>
#include "asd.h"
#include "gamma.h"
#include "video.h"

/*************************************
021092_  Print team scores in UpdateView
021392_  ASD libraries
032592_	Release Version : Team names on Mono Screen,
                        Hotkeys listed
071993_ We're back! (no sir, didn't like it.)
        Attempting to repair the memory hole.
072393_ Cleaning up some of the dirt.

*************************************/

/*Hey FUCKHEAD.  This function here sucks shit!*/
void UpdateView(byte view) {
  static byte oldview;
  char ts[90];
  int x, y, z;
  int rt1 = 0, rt2 = 0, gt1 = 0, gt2 = 0;
  vPage(1);
  if (view != oldview) {
    oldview = view;
    switch (view) {
      case 1: /*REAL TIME POD PERFORMANCE*/ /*RED TEAM*/
      case 6:                               /*GREEN TEAM*/
        for (y = 12; y < 25; ++y)
          for (x = 0; x < 80; ++x) mono(0, y, x) = 32;
        vPosCur(0, 12);
        v_printf(
            " ID -----------³RX STAT³TX STAT³HITS---º ID -----------³RX "
            "STAT³TX STAT³HITS---º");
        vPosCur(0, 13);
        v_printf(
            " #  ID  BSE VAL³RXM RXB³TXM TXB³SNT ACKº #  ID  BSE VAL³RXM "
            "RXB³TXM TXB³SNT ACKº");
        break;
      case 2: /*RADIO VIEW*/
        for (y = 12; y < 25; ++y)
          for (x = 0; x < 80; ++x) mono(0, y, x) = 32;
        vPosCur(0, 12);
        v_printf(
            "    1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20");
        vPosCur(0, 14);
        v_printf("rRX");
        vPosCur(0, 16);
        v_printf("rTX");
        vPosCur(0, 18);
        v_printf("gRX");
        vPosCur(0, 20);
        v_printf("gTX");
        break;
      case 3: /*SCORE VIEW*/
        for (y = 12; y < 25; ++y)
          for (x = 0; x < 80; ++x) mono(0, y, x) = 32;
        if (curconfig.field == 2)
          for (z = 0; z < 10; ++z)
            for (x = 0; x < 10; ++x) {
              mono(0, z + 12, x) = player[z * 2 + 1].name[x];
              mono(0, z + 12, x + 20) = player[z * 2 + 21].name[x];
              mono(0, z + 12, x + 40) = player[z * 2 + 2].name[x];
              mono(0, z + 12, x + 60) = player[z * 2 + 22].name[x];
            }
        else
          for (z = 0; z < 10; ++z)
            for (x = 0; x < 10; ++x) {
              mono(0, z + 12, x) = player[z + 1].name[x];
              mono(0, z + 12, x + 20) = player[z + 11].name[x];
              mono(0, z + 12, x + 40) = player[z + 21].name[x];
              mono(0, z + 12, x + 60) = player[z + 31].name[x];
            }
        break;
      case 5:
        for (y = 12; y < 25; ++y)
          for (x = 0; x < 80; ++x) mono(0, y, x) = 32;
        if (curconfig.field == 2)
          for (z = 0; z < 10; ++z)
            for (x = 0; x < 10; ++x) {
              mono(0, z + 12, x) = player[z * 2 + 1].passport[x];
              mono(0, z + 12, x + 20) = player[z * 2 + 21].passport[x];
              mono(0, z + 12, x + 40) = player[z * 2 + 2].passport[x];
              mono(0, z + 12, x + 60) = player[z * 2 + 22].passport[x];
            }
        else
          for (z = 0; z < 10; ++z)
            for (x = 0; x < 10; ++x) {
              mono(0, z + 12, x) = player[z + 1].passport[x];
              mono(0, z + 12, x + 20) = player[z + 11].passport[x];
              mono(0, z + 12, x + 40) = player[z + 21].passport[x];
              mono(0, z + 12, x + 60) = player[z + 31].passport[x];
            }
        break;
      case 0:
        for (y = 12; y < 25; ++y)
          for (x = 0; x < 80; ++x) mono(0, y, x) = 32;
        break;
    }
  } else {
    switch (view) {
      case 2:
        for (z = 0; z < 20; ++z) {
          vPosCur((byte)(3 + z * 3), 14);
          v_printf("%2x", npoll[z + 5]);
          vPosCur((byte)(3 + z * 3), 18);
          v_printf("%2x", npoll[z + 31]);
        }
        for (z = 0; z < 5; ++z) {
          vPosCur((byte)(3 + z * 3), 16);
          v_printf("%2x", npoll[z + 25]);
          vPosCur((byte)(3 + z * 3), 20);
          v_printf("%2x", npoll[z + 55]);
        }
        break;
      case 3:
      case 5:
        if (curconfig.field == 2) {
          for (z = 0; z < 10; ++z) {
            if (player[z * 2 + 1].used) {
              vPosCur(12, (byte)(z + 12));
              v_printf("%5d", player[z * 2 + 1].score);
              rt1 += player[z * 2 + 1].score;
            }
            if (player[z * 2 + 21].used) {
              vPosCur(32, (byte)(z + 12));
              v_printf("%5d", player[z * 2 + 21].score);
              gt1 += player[z * 2 + 21].score;
            }
            if (player[z * 2 + 2].used) {
              vPosCur(52, (byte)(z + 12));
              v_printf("%5d", player[z * 2 + 2].score);
              rt2 += player[z * 2 + 2].score;
            }
            if (player[z * 2 + 22].used) {
              vPosCur(72, (byte)(z + 12));
              v_printf("%5d", player[z * 2 + 22].score);
              gt2 += player[z * 2 + 22].score;
            }
          }
          vPosCur(12, 22);
          v_printf("%5d", rt1);
          vPosCur(32, 22);
          v_printf("%5d", gt1);
          vPosCur(52, 22);
          v_printf("%5d", rt2);
          vPosCur(72, 22);
          v_printf("%5d", gt2);
        } else {
          for (z = 0; z < 10; ++z) {
            if (player[z + 1].used) {
              vPosCur(12, (byte)(z + 12));
              v_printf("%5d", player[z + 1].score);
              rt1 += player[z + 1].score;
            }
            if (player[z + 11].used) {
              vPosCur(32, (byte)(z + 12));
              v_printf("%5d", player[z + 11].score);
              rt1 += player[z + 11].score;
            }
            if (player[z + 21].used) {
              vPosCur(52, (byte)(z + 12));
              v_printf("%5d", player[z + 21].score);
              gt1 += player[z + 21].score;
            }
            if (player[z + 31].used) {
              vPosCur(72, (byte)(z + 12));
              v_printf("%5d", player[z + 31].score);
              gt1 += player[z + 31].score;
            }
          }
          vPosCur(32, 22);
          v_printf("%5d", rt1);
          vPosCur(72, 22);
          v_printf("%5d", gt1);
        }
        break;
      case 1:
      case 6:
        for (z = 0; z < 20; z += 2) {
          vPosCur(0, (byte)((z / 2) + 14));
          if (view == 1) {
            v_printf(
                "%c%2d %3d %3d %3d³%3d %3d³%3d %3d³%3d %3dº%c%2d %3d %3d "
                "%3d³%3d %3d³%3d %3d³%3d %3d",
                pod[z + 1].resetflag, pod[z + 1].podid, pod[z + 1].id,
                pod[z + 1].base, pod[z + 1].valcode, pod[z + 1].rxmiss,
                pod[z + 1].rxbad, pod[z + 1].txmiss, pod[z + 1].txbad,
                pod[z + 1].hitsent, pod[z + 1].hitackn, pod[z + 2].resetflag,
                pod[z + 2].podid, pod[z + 2].id, pod[z + 2].base,
                pod[z + 2].valcode, pod[z + 2].rxmiss, pod[z + 2].rxbad,
                pod[z + 2].txmiss, pod[z + 2].txbad, pod[z + 2].hitsent,
                pod[z + 2].hitackn);
          } else if (view == 6) {
            v_printf(
                "%c%2d %3d %3d %3d³%3d %3d³%3d %3d³%3d %3dº%c%2d %3d %3d "
                "%3d³%3d %3d³%3d %3d³%3d %3d",
                pod[z + 21].resetflag, pod[z + 21].podid, pod[z + 21].id,
                pod[z + 21].base, pod[z + 21].valcode, pod[z + 21].rxmiss,
                pod[z + 21].rxbad, pod[z + 21].txmiss, pod[z + 21].txbad,
                pod[z + 21].hitsent, pod[z + 21].hitackn, pod[z + 22].resetflag,
                pod[z + 22].podid, pod[z + 22].id, pod[z + 22].base,
                pod[z + 22].valcode, pod[z + 22].rxmiss, pod[z + 22].rxbad,
                pod[z + 22].txmiss, pod[z + 22].txbad, pod[z + 22].hitsent,
                pod[z + 22].hitackn);
          }
        }
    }
  }
}

void DispGameModes(void) {
  if (game.field == 1) {
    vChangeAttr(COLOR(BLK, WHT));
    vPosCur(2, 20);
    v_printf("GAME MODE:");
    DispGameModes2(21, game.mode1, game.redtm1, game.grntm1);
  } else if (game.field == 2) {
    vChangeAttr(COLOR(BLK, WHT));
    vPosCur(2, 19);
    v_printf("ALPHA GAME MODE:");
    vPosCur(2, 21);
    v_printf("OMEGA GAME MODE:");
    DispGameModes2(20, game.mode1, game.redtm1, game.grntm1);
    DispGameModes2(22, game.mode2, game.redtm2, game.grntm2);
  }
}

void DispGameModes2(int row, int mode, char *red, char *grn) {
  int i;
  vChangeAttr(COLOR(BLK, WHT));
  if (mode == PUBLIC) {
    vPosCur(7, (byte)row);
    v_printf("PUBLIC");
  } else if (mode == LEAGUE) {
    vPosCur(7, (byte)row);
    v_printf("LEAGUE -            vs ");
    vPosCur(16, (byte)row);
    v_printf("%10s", red);
    vPosCur(30, (byte)row);
    v_printf("%10s", grn);
  } else if (mode == FREEFORALL) {
    vPosCur(7, (byte)row);
    v_printf("FREE FOR ALL");
  }
}

/*HEY! YOU! FUCKHEAD!  Everything after this line DEFINATLY has to
  change!  It REALLY blows chunks.  Every freking line of code sucks!*/

void SetupMonoGame(void) {
  char ts[80];
  int i, j, k, l;
  for (i = 0; i < 25; ++i)
    for (j = 0; j < 80; ++j) {
      mono(1, i, j) = NORMAL;
      mono(0, i, j) = 32;
    }
  box1(0, 0, 79, 2);
  sprintf(ts, "GAMMA SYSTEMS COMPUTER");
  PrintMono(1, 4, ts);
  sprintf(ts, "ACTIVE - GAME MODE");
  PrintMono(1, 60, ts);
  sprintf(ts, "%s", LOCNAME);
  PrintMono(3, 4, ts);
  sprintf(ts, "%s.%s.%s.%s.%s", STORENUM, SITENUM, OPTNUM, LICNUM, REVNUM);
  PrintMono(3, 55, ts);
  sprintf(ts, "CURRENT MANEUVER:%d", game.number);
  PrintMono(5, 4, ts);
  sprintf(ts, "TIME REMAINING:");
  PrintMono(6, 6, ts);
  sprintf(ts, "SYNC:");
  PrintMono(7, 16, ts);
  for (i = 0; i < 10; ++i) {
    mono(0, 8, 7 + i) = game.redtm1[i];
    mono(0, 8, 21 + i) = game.grntm1[i];
    if ((game.field == 2) && ((curconfig.et1) && (curconfig.et2))) {
      mono(0, 9, 7 + i) = game.redtm2[i];
      mono(0, 9, 21 + i) = game.grntm2[i];
    }
  }

  sprintf(ts, "vs");
  PrintMono(8, 18, ts);
  if ((game.field == 2) && ((curconfig.et1) && (curconfig.et2)))
    PrintMono(9, 18, ts);
  sprintf(ts, "TEST SLOT1");
  PrintMono(5, 29, ts);
  sprintf(ts, "NO LOAD Pod");
  PrintMono(6, 28, ts);
  sprintf(ts, "Hit CTRL-E to END GAME");
  PrintMono(5, 54, ts);
  sprintf(ts, "s = Scores w/ codenames");
  PrintMono(6, 54, ts);
  sprintf(ts, "c = Scores w/passports");
  PrintMono(7, 54, ts);
  sprintf(ts, "p = Red Pod Performance");
  PrintMono(8, 54, ts);
  sprintf(ts, "P = Grn Pod Performance");
  PrintMono(9, 54, ts);
  sprintf(ts, "n = Clear View Window");
  PrintMono(10, 54, ts);
  for (i = 0; i < 80; ++i) mono(0, 11, i) = 'Í';
}

void SetupMonoIdle(byte again) {
  char ts[80];
  char tmp[10][36];
  int i, j, k, l;
  if (again == 1)
    for (j = 0; j < 10; ++j)
      for (i = 0; i < 36; ++i) tmp[j][i] = mono(0, j + 12, i + 1);
  for (i = 0; i < 25; ++i)
    for (j = 0; j < 80; ++j) {
      mono(1, i, j) = NORMAL;
      mono(0, i, j) = 32;
    }
  if (again == 1)
    for (j = 0; j < 10; ++j)
      for (i = 0; i < 36; ++i) mono(0, j + 12, i + 1) = tmp[j][i];
  box1(0, 0, 79, 2);
  sprintf(ts, "GAMMA SYSTEMS COMPUTER");
  PrintMono(1, 4, ts);
  sprintf(ts, "IDLE - GAME MODE");
  PrintMono(1, 60, ts);
  sprintf(ts, "%s", LOCNAME);
  PrintMono(3, 4, ts);
  sprintf(ts, "%s.%s.%s.%s.%s", STORENUM, SITENUM, OPTNUM, LICNUM, REVNUM);
  PrintMono(3, 55, ts);
  sprintf(ts, "NEXT MANEUVER:%d", game.number);
  PrintMono(5, 4, ts);
  sprintf(ts, "NEXT TRACK:");
  PrintMono(6, 7, ts);
  sprintf(ts, "SYNC:");
  PrintMono(7, 13, ts);
  box2(0, 11, 37, 22);
  box2(50, 5, 79, 9);
  box2(50, 12, 79, 16);
  sprintf(ts, "SYSTEM INFO");
  PrintMono(11, 1, ts);
  sprintf(ts, "ET1 STATUS");
  PrintMono(5, 51, ts);
  sprintf(ts, "ET2 STATUS");
  PrintMono(12, 51, ts);
  sprintf(
      ts,
      "CTRL-Q TO EXIT GAME MODE, L TO PRINT LAST GAME, [#] TO SELECT CD TRACK");
  PrintMono(24, 0, ts);
}

void box2(int x, int y, int i, int j) {
  int l, m;
  char tr = '»';
  char tl = 'É';
  char br = '¼';
  char bl = 'È';
  char hr = 'Í';
  char vr = 'º';
  for (l = x; l <= i; ++l) {
    mono(0, y, l) = hr;
    mono(0, j, l) = hr;
  }
  for (l = y; l <= j; ++l) {
    mono(0, l, x) = vr;
    mono(0, l, i) = vr;
  }
  mono(0, y, x) = tl;
  mono(0, j, i) = br;
  mono(0, j, x) = bl;
  mono(0, y, i) = tr;
}

/*Gads, your mom lets you write code like that?*/

void info(char *data) {
  int x, y;
  for (y = 12; y < 21; ++y)
    for (x = 1; x <= 36; ++x) mono(0, y, x) = (mono(0, (y + 1), x));
  for (x = 1; x < 36; ++x) mono(0, 21, x) = 32;
  PrintMono(21, 1, data);
}

void UpdateMonoIdle(byte rec1, byte rec2, byte track) {
  static byte orec1, orec2;
  char ts[40];
  sprintf(ts, "%d", track);
  PrintMono(6, 18, ts);
  sprintf(ts, "%2d-%02d-%02d      %2d:%02d:%02d", curdate.month, curdate.day,
          curdate.year, curtime.hour, curtime.minute, curtime.second);
  PrintMono(3, 25, ts);
  if (orec1 != rec1) {
    PrintEtStatus(8, 51, rec1);
    orec1 = rec1;
  }
  if (orec2 != rec2) {
    PrintEtStatus(15, 51, rec2);
    orec2 = rec2;
  }
  UpdateSync(7, 18, sync);
}

void PrintEtStatus(int x, int y, byte rec) {
  int i;
  char ts[40];
  char *SSS;

  switch (rec) {
    case MAINMENU:
      SSS = "MAIN MENU";
      break;
    case EDITMODE:
      SSS = "EDIT GAME";
      break;
    case T_E_M:
      SSS = "T/E/M MENU";
      break;
    case S_E:
      SSS = "S/E MENU";
      break;
    case C_E_M:
      SSS = "C/E/M MENU";
      break;
    case REQXFER:
      SSS = "REQUESTING XFER";
      break;
    case WAITGC:
      SSS = "AWAITING GC START";
      break;
    case START:
      SSS = "AWAITING GAME START ACKN";
      break;
    case SENDRED:
    case SENDGRN:
      SSS = "SENDING DATA";
      break;
    case 0xdd:
      SSS = "INACTIVE";
      break;
    default:
      SSS = "???";
      break;
  }
  for (i = 51; i <= 78; ++i) {
    mono(0, x, i) = 32;
  }
  PrintMono(x, y, SSS);
}

void UpdateSync(int y, int x, byte sync) {
  switch (sync) {
    case GSYNC:
      mono(0, y, x) = 'G';
      break;
    case PSYNC:
      mono(0, y, x) = 'P';
      break;
    case ESYNC:
      mono(0, y, x) = 'E';
      break;
    case NSYNC:
      mono(0, y, x) = 'N';
      break;
  }
}

/*Let's get some kind of really spiffy array in here.
  Maybe even load a file in during init.
*/

void CheckPassport(char *pnum) {
  if ((strncmp(pnum, "0000110001", 10)) == 0)
    NewTrack(1);
  else if ((strncmp(pnum, "0000110002", 10)) == 0)
    NewTrack(2);
  else if ((strncmp(pnum, "0000110003", 10)) == 0)
    NewTrack(3);
  else if ((strncmp(pnum, "0106000000", 10)) == 0)
    NewTrack(4);
  else if ((strncmp(pnum, "0106029028", 10)) == 0)
    NewTrack(6);
  else if ((strncmp(pnum, "0000110006", 10)) == 0)
    NewTrack(6);
  else if ((strncmp(pnum, "0000110007", 10)) == 0)
    NewTrack(7);
  else if ((strncmp(pnum, "0000110008", 10)) == 0)
    NewTrack(8);
  else if ((strncmp(pnum, "0187055945", 10)) == 0)
    NewTrack(8);
}

void NewTrack(byte trackno) {
  gametrack = trackno;
  CD_track(gametrack); /*send track to cd*/
  if (curconfig.ec == TRUE) {
    effectsout("\b\b\b", 3); /*back space old ru command*/
    settrack();              /*send ru(x) to effects*/
  }
}
