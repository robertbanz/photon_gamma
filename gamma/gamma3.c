/*************
 gamma3.c
 *************/
#define SYSTEM_PROGRAM
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

void UpdateView(byte view) {
  static byte oldview;
  char ts[90];
  int x, y, z;
  int rt1 = 0, rt2 = 0, gt1 = 0, gt2 = 0;
  SelectIO(CONS);
  if (view != oldview) {
    oldview = view;
    switch (view) {
      case 1: /*REAL TIME POD PERFORMANCE*/ /*RED TEAM*/
      case 6:                               /*GREEN TEAM*/
        vPosCur(0, 12);
        vBox(0xffff, ' ', 80, 12);
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
        vPosCur(0, 12);
        vBox(0xffff, ' ', 80, 12);
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
        vPosCur(0, 12);
        vBox(0xffff, ' ', 80, 12);
        if (curconfig.field == 2)
          for (z = 0; z < 10; ++z) {
            vPosCur(0, z + 12);
            v_sendsn(player[z * 2 + 1].name, 10);
            vPosCur(20, z + 12);
            v_sendsn(player[z * 2 + 21].name, 10);
            vPosCur(40, z + 12);
            v_sendsn(player[z * 2 + 2].name, 10);
            vPosCur(60, z + 12);
            v_sendsn(player[z * 2 + 22].name, 10);
          }
        else
          for (z = 0; z < 10; ++z) {
            vPosCur(0, z + 12);
            v_sendsn(player[z + 1].name, 10);
            vPosCur(20, z + 12);
            v_sendsn(player[z + 11].name, 10);
            vPosCur(40, z + 12);
            v_sendsn(player[z + 21].name, 10);
            vPosCur(60, z + 12);
            v_sendsn(player[z + 31].name, 10);
          }
        break;
      case 5:
        vPosCur(0, 12);
        vBox(0xffff, ' ', 80, 12);
        if (curconfig.field == 2)
          for (z = 0; z < 10; ++z) {
            vPosCur(0, z + 12);
            v_sendsn(player[z * 2 + 1].passport, 10);
            vPosCur(20, z + 12);
            v_sendsn(player[z * 2 + 21].passport, 10);
            vPosCur(40, z + 12);
            v_sendsn(player[z * 2 + 2].passport, 10);
            vPosCur(60, z + 12);
            v_sendsn(player[z * 2 + 22].passport, 10);
          }
        else
          for (z = 0; z < 10; ++z) {
            vPosCur(0, z + 12);
            v_sendsn(player[z + 1].passport, 10);
            vPosCur(20, z + 12);
            v_sendsn(player[z + 11].passport, 10);
            vPosCur(40, z + 12);
            v_sendsn(player[z + 21].passport, 10);
            vPosCur(60, z + 12);
            v_sendsn(player[z + 31].passport, 10);
          }
        break;
      case 0:
        vPosCur(0, 12);
        vBox(0xffff, ' ', 80, 12);
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

void SetupMonoGame(void) {
  char ts[80];
  int i, j, k, l;
  vPage(1);
  vPosCur(0, 0);
  vBox(0xffff, ' ', 80, 25);
  vBorder(80, 3, 0);
  vPosCur(4, 1);
  v_sends("Gamma Systems Computer");
  vPosCur(60, 1);
  v_sends("ACTIVE - GAME MODE");
  vPosCur(4, 3);
  v_sends(LOCNAME);
  vPosCur(4, 5);
  v_printf("Current Maneuver:%d", game.number);
  vPosCur(6, 6);
  v_sends("Time Remaining:");
  vPosCur(16, 7);
  v_sends("Sync:");
  vPosCur(7, 8);
  v_sendsn(game.redtm1, 10);
  vPosCur(21, 8);
  v_sendsn(game.grntm1, 10);
  if ((game.field == 2) && ((curconfig.et1) && (curconfig.et2))) {
    vPosCur(7, 9);
    v_sendsn(game.redtm2, 10);
    vPosCur(21, 9);
    v_sendsn(game.grntm2, 10);
    vPosCur(18, 9);
    v_sends("vs");
  }
  vPosCur(18, 8);
  v_sends("vs");
  vPosCur(29, 5);
  v_sends("Test Slot 1");
  vPosCur(28, 6);
  v_sends("No Load Pod");
  vPosCur(54, 5);
  v_sends("Hit CTRL-E to END GAME");
  vPosCur(54, 6);
  v_sends("s = Scores w/ codenames");
  vPosCur(54, 7);
  v_sends("c = Scores w/ passports");
  vPosCur(54, 8);
  v_sends("p = Red Pod Performance");
  vPosCur(54, 9);
  v_sends("P = Grn Pod Performance");
  vPosCur(54, 10);
  v_sends("n = Clear View Window");
  vPosCur(0, 11);
  vRepeat('Í', 80);
  vStatLine("", 0, 0, 1);
}

void SetupMonoIdle(byte again) {
  char ts[80];
  char tmp[10][36];
  int i, j, k, l;
  /*truly dirty portion of code I'm leaving for now...*/
  vPage(1);
  if (again == 1)
    for (j = 0; j < 10; ++j)
      for (i = 0; i < 36; ++i) tmp[j][i] = mono(0, j + 12, i + 1);
  vPage(1);
  vPosCur(0, 0);
  vBox(0xffff, ' ', 80, 25);

  /*the second half of the dirty code*/
  if (again == 1)
    for (j = 0; j < 10; ++j)
      for (i = 0; i < 36; ++i) mono(0, j + 12, i + 1) = tmp[j][i];

  /*the dirty code is all over*/

  vPosCur(0, 0);
  vBorder(80, 3, 0);
  vPosCur(4, 1);
  v_sends("GAMMA SYSTEMS COMPUTER");
  vPosCur(60, 1);
  v_sends("IDLE - GAME MODE");
  vPosCur(4, 3);
  v_sends(LOCNAME);
  vPosCur(4, 5);
  v_printf("Next Maneuver:%d", game.number);
  vPosCur(7, 6);
  v_printf("Next Track:");
  vPosCur(13, 7);
  v_printf("Sync:");
  vPosCur(0, 11);
  vBorder(38, 12, 3);
  vPosCur(50, 5);
  vBorder(30, 5, 3);
  vPosCur(50, 12);
  vBorder(30, 5, 3);
  vPosCur(1, 11);
  v_sends("SYSTEM INFO");
  vPosCur(51, 5);
  v_sends("ET1 STATUS");
  vPosCur(51, 12);
  v_sends("ET2 STATUS");
  vStatLine(
      "CTRL-Q TO EXIT GAME MODE, L TO PRINT LAST GAME, [#] TO SELECT CD TRACK",
      0, COLOR(WHT, BLK), 1);
}

/*this stays the way it is right now...*/
/*it works, it's small...not portalbe, but probably faster!*/
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
  vPage(1);

  vPosCur(18, 6);

  v_printf("%d", track);

  sprintf(ts, "%2d-%02d-%02d      %2d:%02d:%02d", curdate.month, curdate.day,
          curdate.year, curtime.hour, curtime.minute, curtime.second);
  vPosCur(25, 3);
  v_sends(ts);

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
  vPosCur(51, x);
  vBox(0xffff, ' ', 28, 1);
  vPosCur(y, x);
  v_sends(SSS);
}

void UpdateSync(int y, int x, byte sync) {
  vPage(1);
  vPosCur(x, y);
  switch (sync) {
    case GSYNC:
      vPutch('G');
      break;
    case PSYNC:
      vPutch('P');
      break;
    case ESYNC:
      vPutch('E');
      break;
    case NSYNC:
      vPutch('N');
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
