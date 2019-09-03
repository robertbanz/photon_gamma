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
#include "outctl.h"
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
  SelectIO(&CONS);
  if (view != oldview) {
    oldview = view;
    switch (view) {
      case 1: /*REAL TIME POD PERFORMANCE*/ /*RED TEAM*/
      case 6:                               /*GREEN TEAM*/
        sPositionCur(0, 12);
        fBox(0xffff, ' ', 80, 12);
        sPositionCur(0, 12);
        aprintf(
            " ID -----------³RX STAT³TX STAT³HITS---º ID -----------³RX "
            "STAT³TX STAT³HITS---º");
        sPositionCur(0, 13);
        aprintf(
            " #  ID  BSE VAL³RXM RXB³TXM TXB³SNT ACKº #  ID  BSE VAL³RXM "
            "RXB³TXM TXB³SNT ACKº");
        break;
      case 2: /*RADIO VIEW*/
        sPositionCur(0, 12);
        fBox(0xffff, ' ', 80, 12);
        sPositionCur(0, 12);
        aprintf(
            "    1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20");
        sPositionCur(0, 14);
        aprintf("rRX");
        sPositionCur(0, 16);
        aprintf("rTX");
        sPositionCur(0, 18);
        aprintf("gRX");
        sPositionCur(0, 20);
        aprintf("gTX");
        break;
      case 3: /*SCORE VIEW*/
        sPositionCur(0, 12);
        fBox(0xffff, ' ', 80, 12);
        if (curconfig.field == 2)
          for (z = 0; z < 10; ++z) {
            sPositionCur(0, z + 12);
            asendc(player[z * 2 + 1].name, 10);
            sPositionCur(20, z + 12);
            asendc(player[z * 2 + 21].name, 10);
            sPositionCur(40, z + 12);
            asendc(player[z * 2 + 2].name, 10);
            sPositionCur(60, z + 12);
            asendc(player[z * 2 + 22].name, 10);
          }
        else
          for (z = 0; z < 10; ++z) {
            sPositionCur(0, z + 12);
            asendc(player[z + 1].name, 10);
            sPositionCur(20, z + 12);
            asendc(player[z + 11].name, 10);
            sPositionCur(40, z + 12);
            asendc(player[z + 21].name, 10);
            sPositionCur(60, z + 12);
            asendc(player[z + 31].name, 10);
          }
        break;
      case 5:
        sPositionCur(0, 12);
        fBox(0xffff, ' ', 80, 12);
        if (curconfig.field == 2)
          for (z = 0; z < 10; ++z) {
            sPositionCur(0, z + 12);
            asendc(player[z * 2 + 1].passport, 10);
            sPositionCur(20, z + 12);
            asendc(player[z * 2 + 21].passport, 10);
            sPositionCur(40, z + 12);
            asendc(player[z * 2 + 2].passport, 10);
            sPositionCur(60, z + 12);
            asendc(player[z * 2 + 22].passport, 10);
          }
        else
          for (z = 0; z < 10; ++z) {
            sPositionCur(0, z + 12);
            asendc(player[z + 1].passport, 10);
            sPositionCur(20, z + 12);
            asendc(player[z + 11].passport, 10);
            sPositionCur(40, z + 12);
            asendc(player[z + 21].passport, 10);
            sPositionCur(60, z + 12);
            asendc(player[z + 31].passport, 10);
          }
        break;
      case 0:
        sPositionCur(0, 12);
        fBox(0xffff, ' ', 80, 12);
        break;
    }
  } else {
    switch (view) {
      case 2:
        for (z = 0; z < 20; ++z) {
          sPositionCur((byte)(3 + z * 3), 14);
          aprintf("%2x", npoll[z + 5]);
          sPositionCur((byte)(3 + z * 3), 18);
          aprintf("%2x", npoll[z + 31]);
        }
        for (z = 0; z < 5; ++z) {
          sPositionCur((byte)(3 + z * 3), 16);
          aprintf("%2x", npoll[z + 25]);
          sPositionCur((byte)(3 + z * 3), 20);
          aprintf("%2x", npoll[z + 55]);
        }
        break;
      case 3:
      case 5:
        if (curconfig.field == 2) {
          for (z = 0; z < 10; ++z) {
            if (player[z * 2 + 1].used) {
              sPositionCur(12, (byte)(z + 12));
              aprintf("%5d", player[z * 2 + 1].score);
              rt1 += player[z * 2 + 1].score;
            }
            if (player[z * 2 + 21].used) {
              sPositionCur(32, (byte)(z + 12));
              aprintf("%5d", player[z * 2 + 21].score);
              gt1 += player[z * 2 + 21].score;
            }
            if (player[z * 2 + 2].used) {
              sPositionCur(52, (byte)(z + 12));
              aprintf("%5d", player[z * 2 + 2].score);
              rt2 += player[z * 2 + 2].score;
            }
            if (player[z * 2 + 22].used) {
              sPositionCur(72, (byte)(z + 12));
              aprintf("%5d", player[z * 2 + 22].score);
              gt2 += player[z * 2 + 22].score;
            }
          }
          sPositionCur(12, 22);
          aprintf("%5d", rt1);
          sPositionCur(32, 22);
          aprintf("%5d", gt1);
          sPositionCur(52, 22);
          aprintf("%5d", rt2);
          sPositionCur(72, 22);
          aprintf("%5d", gt2);
        } else {
          for (z = 0; z < 10; ++z) {
            if (player[z + 1].used) {
              sPositionCur(12, (byte)(z + 12));
              aprintf("%5d", player[z + 1].score);
              rt1 += player[z + 1].score;
            }
            if (player[z + 11].used) {
              sPositionCur(32, (byte)(z + 12));
              aprintf("%5d", player[z + 11].score);
              rt1 += player[z + 11].score;
            }
            if (player[z + 21].used) {
              sPositionCur(52, (byte)(z + 12));
              aprintf("%5d", player[z + 21].score);
              gt1 += player[z + 21].score;
            }
            if (player[z + 31].used) {
              sPositionCur(72, (byte)(z + 12));
              aprintf("%5d", player[z + 31].score);
              gt1 += player[z + 31].score;
            }
          }
          sPositionCur(32, 22);
          aprintf("%5d", rt1);
          sPositionCur(72, 22);
          aprintf("%5d", gt1);
        }
        break;
      case 1:
      case 6:
        for (z = 0; z < 20; z += 2) {
          sPositionCur(0, (byte)((z / 2) + 14));
          if (view == 1) {
            aprintf(
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
            aprintf(
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
    sChangeAttr(COLOR(BLK, WHT));
    sPositionCur(2, 20);
    aprintf("GAME MODE:");
    DispGameModes2(21, game.mode1, game.redtm1, game.grntm1);
  } else if (game.field == 2) {
    sChangeAttr(COLOR(BLK, WHT));
    sPositionCur(2, 19);
    aprintf("ALPHA GAME MODE:");
    sPositionCur(2, 21);
    aprintf("OMEGA GAME MODE:");
    DispGameModes2(20, game.mode1, game.redtm1, game.grntm1);
    DispGameModes2(22, game.mode2, game.redtm2, game.grntm2);
  }
}

void DispGameModes2(int row, int mode, char *red, char *grn) {
  int i;
  sChangeAttr(COLOR(BLK, WHT));
  if (mode == PUBLIC) {
    sPositionCur(7, (byte)row);
    aprintf("PUBLIC");
  } else if (mode == LEAGUE) {
    sPositionCur(7, (byte)row);
    aprintf("LEAGUE -            vs ");
    sPositionCur(16, (byte)row);
    aprintf("%10s", red);
    sPositionCur(30, (byte)row);
    aprintf("%10s", grn);
  } else if (mode == FREEFORALL) {
    sPositionCur(7, (byte)row);
    aprintf("FREE FOR ALL");
  }
}

void SetupMonoGame(void) {
  char ts[80];
  int i, j, k, l;
  SelectIO(&CONS);
  sPositionCur(0, 0);
  fBox(0xffff, ' ', 80, 25);
  sBorder(80, 3, 0);
  sPositionCur(4, 1);
  asends("Gamma Systems Computer");
  sPositionCur(60, 1);
  asends("ACTIVE - GAME MODE");
  sPositionCur(4, 3);
  asends(LOCNAME);
  sPositionCur(4, 5);
  aprintf("Current Maneuver:%d", game.number);
  sPositionCur(6, 6);
  asends("Time Remaining:");
  sPositionCur(16, 7);
  asends("Sync:");
  sPositionCur(7, 8);
  asendc(game.redtm1, 10);
  sPositionCur(21, 8);
  asendc(game.grntm1, 10);
  if ((game.field == 2) && ((curconfig.et1) && (curconfig.et2))) {
    sPositionCur(7, 9);
    asendc(game.redtm2, 10);
    sPositionCur(21, 9);
    asendc(game.grntm2, 10);
    sPositionCur(18, 9);
    asends("vs");
  }
  sPositionCur(18, 8);
  asends("vs");
  sPositionCur(29, 5);
  asends("Test Slot 1");
  sPositionCur(28, 6);
  asends("No Load Pod");
  sPositionCur(54, 5);
  asends("Hit CTRL-E to END GAME");
  sPositionCur(54, 6);
  asends("s = Scores w/ codenames");
  sPositionCur(54, 7);
  asends("c = Scores w/ passports");
  sPositionCur(54, 8);
  asends("p = Red Pod Performance");
  sPositionCur(54, 9);
  asends("P = Grn Pod Performance");
  sPositionCur(54, 10);
  asends("n = Clear View Window");
  sPositionCur(0, 11);
  sRepeatChar('Í', 80);
  vStatLine("", 0, 0, 1);
}

struct {
  int crap;
} m_s_d;

void cons_statusblock(int x, int y, int z) {}

void cons_printstatus(int x, int y, int z) {}

void cons_printtime(int x, int y, int z) {}

void cons_cdstatus(int x, int y, int z) {}

char *GetEtStatus(byte rec) {
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
  return SSS;
}

void cons_etblock(int x, int y, int z) {
  /*
      if ((z) || (GetEtStatus(umi1) != m_s_d.et1)) {
          sPositionCur(x+0,y+1);
          sChangeAttr(COLOR(HBRN,BLU));
          asends(GetEtStatus(umi1));
          m_s_d.et1 = GetEtStatus(umi1);
  */
}

void SetupMonoIdle(byte again) {
  SelectIO(&CONS);
  sPositionCur(0, 0);
  fBox(0xffff, ' ', 80, 25);

  sPositionCur(0, 0);
  sChangeAttr(COLOR(HWHT, BLU));
  fBox(0xffff, ' ', 80, 1);
  sPositionCur(2, 0);
  asends("Gamma Systems Computer");
  sPositionCur(30, 0);
  asends(LOCNAME);

  cons_printtime(56, 0, 1);

  sPositionCur(0, 1);
  sChangeAttr(COLOR(HBLU, BLU));
  fBox(0xffff, '²', 80, 14);

  sChangeAttr(COLOR(BLK, WHT));
  sPositionCur(2, 2);
  fBox(0xffff, ' ', 40, 1);

  cons_printstatus(2, 2, 1);

  sChangeAttr(COLOR(HBLK, BLU));
  sPositionCur(3, 5);
  fBox(0xffff, '±', 22, 4);
  sChangeAttr(COLOR(HWHT, BLU));
  sPositionCur(2, 4);
  fBox(0xffff, ' ', 22, 4);

  cons_statusblock(2, 4, 1);

  sChangeAttr(COLOR(HBLK, BLU));
  sPositionCur(3, 11);
  fBox(0xffff, '±', 22, 4);
  sChangeAttr(COLOR(HWHT, BLU));
  sPositionCur(2, 10);
  fBox(0xffff, ' ', 22, 4);

  cons_etblock(2, 10, 1);

  sChangeAttr(COLOR(HBLK, BLU));
  sPositionCur(53, 12);
  fBox(0xffff, '±', 25, 2);
  sChangeAttr(COLOR(HWHT, BLU));
  sPositionCur(52, 11);
  fBox(0xffff, ' ', 25, 2);
  sPositionCur(52, 11);
  asends("CDROM STATUS");
  sPositionCur(65, 11);
  sRepeatChar('Í', 12);

  cons_cdstatus(66, 13, 1);

  sPositionCur(0, 15);
  sChangeAttr(COLOR(HWHT, BLU));
  fBox(0xffff, ' ', 80, 9);
  sPositionCur(0, 15);
  asends("System Messages");
  sPositionCur(16, 15);
  sRepeatChar('Í', 64);

  sPositionCur(51, 4);
  sChangeAttr(COLOR(HBLK, BLU));
  fBox(0xffff, '±', 26, 7);
  sPositionCur(50, 3);
  sChangeAttr(COLOR(HWHT, BLU));
  fBox(0xffff, ' ', 26, 7);
  sPositionCur(50, 3);
  asends("System Controls");
  sPositionCur(66, 3);
  sRepeatChar('Í', 10);
  sPositionCur(50, 6);
  asends("CD Controls ");
  sRepeatChar('Í', 14);
  sChangeAttr(COLOR(WHT, BLU));
  sPositionCur(50, 4);
  asends("CTRL-R  Configuation Menu");
  sPositionCur(50, 5);
  asends("#'s     Choose Game Track");
  sPositionCur(50, 7);
  asends("<,>     Change Tracks");
  sPositionCur(50, 8);
  asends("=       Stop");
  sPositionCur(50, 9);
  asends("/       Play");
  SelectIO(&PUB);
}

/*this stays the way it is right now...*/
/*it works, it's small...not portalbe, but probably faster!*/
void info(char *data) {
  /*int x,y;
  for(y=12;y<21;++y)
          for(x=1;x<=36;++x)
                  mono(0,y,x) = (mono(0,(y+1),x));
  for(x=1;x<36;++x)
          mono(0,21,x) = 32;
  PrintMono(21,1,data); shah...as if...*/
}

void UpdateMonoIdle(byte rec1, byte rec2, byte track) {
  static oslot;
  static byte orec1, orec2;
  char ts[40];
  if (oslot != slot) return;
  oslot = slot;

  SelectIO(&CONS);

  sPositionCur(18, 6);

  aprintf("%d", track);

  sprintf(ts, "%2d-%02d-%02d      %2d:%02d:%02d", curdate.month, curdate.day,
          curdate.year, curtime.hour, curtime.minute, curtime.second);
  sPositionCur(25, 3);
  asends(ts);

  if (orec1 != rec1) {
    /*		PrintEtStatus(8,51,rec1);
                    orec1 = rec1;*/
  }
  if (orec2 != rec2) {
    /*		PrintEtStatus(15,51,rec2);
                    orec2 = rec2;*/
  }
  UpdateSync(7, 18, sync);
}

void UpdateSync(int y, int x, byte sync) {
  SelectIO(&CONS);
  sPositionCur(x, y);
  switch (sync) {
    case GSYNC:
      asend('G');
      break;
    case PSYNC:
      asend('P');
      break;
    case ESYNC:
      asend('E');
      break;
    case NSYNC:
      asend('N');
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
