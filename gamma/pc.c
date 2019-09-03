/************
 pc.c
 ************/

/*481234-4.asd

MMYYHHMM.NNT*/

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include "asd.h"
#include "asdmenu.h"
#include "video.h"
typedef unsigned char byte;
/******************************************
Gamma System Progress Computer DISPLAY SERVER
Version .1 021392	  File Created.
Version .2 041092   Hmmm...it got better?
Version .3 041592   "Tournament" mode added
Version .4 041792   "Tournament" mode actually did something!
Version .5 041892   Fixed double printing fuck ups.
Version .6 060692   Fixed "eat me" mode when game is longer than memory buffer
*******************************************/
/*******************************************
Uses display routines created by Chris Fanning
********************************************/

void setupslots(void);
struct dostime_t curtime; /*current time*/
struct dosdate_t curdate; /*current date*/

FILE *gamelist, *gdata;

void HOST_recvsn(unsigned int port, char *buffer, size_t size);

#pragma pack(1)
struct gamestruct {
  char redtm1[10], grntm1[10], redtm2[10], grntm2[10];
  byte field;
  byte mode1, mode2;
  byte hcp_a, hcp_b;
  int number;
  int length;
} game;
#pragma pack()

#pragma pack(1)
struct podstruct {
  int id, base, rxmiss, rxbad, txmiss, txbad, valcode, hitackn, hitsent;
  byte podid;
  byte missinrow;
  byte resetflag;
} pod[41];
#pragma pack()

struct com_5 {
  unsigned DR : 1;
  unsigned OE : 1;
  unsigned PE : 1;
  unsigned FE : 1;
  unsigned BI : 1;
  unsigned THRE : 1;
  unsigned TSRE : 1;
  unsigned NOT : 1;
};

struct playrinfo {
  char playername[10];
  int hitopp[40];
  char baseflag;
  char used;
  char reset;
} pl[40];

int score[40];
#define INIT 0xE1
#define SENDDATA 0xE2
#define GAMEMODE 0xE3
#define STARTPOLL 0xE4
#define ENDPOLL 0xE5
#define ENQMODE 0xE6
#define CLEAR 0xE7
#define SAVE 0xE9

#define COM 0x2F8

#define ReadPort(x, y)  \
  {                     \
    incoming = y;       \
    *incoming = inp(x); \
  }

unsigned char irslot[256];
unsigned char slotir[256];
unsigned char *incoming;
unsigned char tmode;
unsigned char GameData[360][40];
struct com_5 pinfo;

void ChgColor(int i) {
  if (i < 20)
    vChangeAttr(COLOR(RED, BLK));
  else
    vChangeAttr(COLOR(GRN, BLK));
}

main() {
  int i;
  int j;
  int pollnum = 0;
  int fflag;
  int noinfo;
  int quit = 0;
  int l;
  byte temp;
  char ftype;
  char gfile[40];
  char ts[40];
  unsigned char din[40];
  struct com_5 pinfo;
  setupslots();
  /*****INIT COM PORT****/
  tmode = 0;

  outp(COM + 3, 128);
  outp(COM, 0x0c);
  outp(COM + 1, 0x00);
  outp(COM + 3, 3);
  ASDInit(CGA);
  vClearScreen();
  vChangeAttr(COLOR(WHT, BLK));
  vPosCur(1, 1);
  asends("Copyright (c) 1991,92 Robert Banz, ");
  vCR();
  vLF();
  asends("All rights reserved");
  vCR();
  vLF();
  vStatLine("     GAMMA PC 041892 V0.5", 0, COLOR(WHT, BLU), 1);
  while (quit == 0) {
    if (kbhit()) switch (getch()) {
        case 17:
          exit(-1);
          break;
      }
    ReadPort(COM + 5, &pinfo);
    if (pinfo.DR == 1) {
      temp = inp(COM);
    inter:
      switch (temp) {
        case SAVE:
          gamelist = fopen("glist.dat", "a");
          sprintf(gfile, "%02d%02d%02d%02d.%02dG", curdate.month, curdate.day,
                  curtime.hour, curtime.minute, game.number);
          gdata = fopen(gfile, "wb");
          fprintf(gamelist, "%s :", gfile);
          for (i = 0; i < 10; ++i) fprintf(gamelist, "%c", game.redtm1[i]);
          fprintf(gamelist, " vs ");
          for (i = 0; i < 10; ++i) fprintf(gamelist, "%c", game.grntm1[i]);
          fprintf(gamelist, "\n");
          fclose(gamelist);
          fwrite(&curtime, sizeof(curtime), 1, gdata);
          fwrite(&curdate, sizeof(curdate), 1, gdata);
          for (l = 1; l < 41; ++l) {
            fwrite(&score[l], sizeof(int), 1, gdata);
            fwrite(&pod[l], sizeof(struct podstruct), 1, gdata);
          }
          fwrite(&game, sizeof(struct gamestruct), 1, gdata);
          fwrite(pl, sizeof(struct playrinfo), 40, gdata);
          fwrite(&GameData, sizeof(byte), (360 * 40), gdata);
          fclose(gdata);
          break;
        case INIT:
          vClearScreen();
          vStatLine("Boot Mode", 4, COLOR(WHT, BLU), 1);
          break;
        case GAMEMODE:
          vClearScreen();
          vStatLine("Game Mode", 4, COLOR(WHT, BLU), 1);
          pollnum = 0;
          for (l = 0; l < 360; ++l)
            for (i = 0; i < 40; ++i) GameData[l][i] = 0;

          break;
        case SENDDATA:
          for (i = 0; i < 40; ++i) {
            for (j = 0; j < 10; ++j) {
              do {
                ReadPort(COM + 5, &pinfo);
              } while (pinfo.DR == 0);
              pl[i].playername[j] = (char)inp(COM);
            }
            do {
              ReadPort(COM + 5, &pinfo);
            } while (pinfo.DR == 0);
            pl[i].used = (char)inp(COM);
          }
          vStatLine("Data Received...", 4, COLOR(WHT, BLU), 1);
          fflag = 1;
          for (i = 0; i < 40; ++i) {
            pl[i].baseflag = 0;
            pl[i].reset = 0;
          }
          break;
        case CLEAR:
          vStatLine("Game <<<TERMINATED>>>", 4, COLOR(WHT, BLU), 1);
          break;
        case STARTPOLL:
          fflag = 0;
          for (i = 0; i < 40; ++i) {
            do {
              ReadPort(COM + 5, &pinfo);
            } while (pinfo.DR == 0);
            din[i] = (char)inp(COM);
          }
          break;
        case ENQMODE:
          if (1 == 1) {
            vStatLine("Reading Game Results", 4, COLOR(WHT, BLU), 1);

            HOST_recvsn(COM, &curtime, sizeof(curtime));
            HOST_recvsn(COM, &curdate, sizeof(curdate));
            for (l = 1; l < 41; ++l) {
              HOST_recvsn(COM, &score[l], sizeof(int));
              HOST_recvsn(COM, &pod[l], sizeof(struct podstruct));
            }
            HOST_recvsn(COM, &game, sizeof(struct gamestruct));
          } else
            outp(COM, 0xEF);
          break;
        case ENDPOLL:
          noinfo = 0;
          if (fflag == 1)
            fflag = 0;
          else {
            sprintf(ts, "%04d", pollnum++);
            vStatLine(ts, 30, COLOR(WHT, BLU), 1);
            if (pollnum < 359) memmove(&GameData[pollnum][0], din, 40);
            for (i = 0; i < 40; ++i) {
              if (pl[i].used == 1) {
                if ((pl[i].reset == 0) && (din[i] == 0xEE)) {
                  if (noinfo == 0) {
                    vCR();
                    vLF();
                  }
                  ++noinfo;
                  pl[i].reset = 1;
                  asends("   ");
                  ChgColor(i);
                  asendc(pl[i].playername, 10);
                  vChangeAttr(COLOR(WHT, BLK));
                  asends(" is reset");
                  vCR();
                  vLF();
                }

                else if ((pl[i].baseflag == 0) &&
                         ((din[i] == 0x35) || (din[i] == 0x2B))) {
                  if (noinfo == 0) {
                    vCR();
                    vLF();
                  }
                  ++noinfo;
                  pl[i].baseflag = 1;
                  asends("   ");
                  ChgColor(i);
                  asendc(pl[i].playername, 10);
                  vChangeAttr(COLOR(WHT, BLK));
                  asends(" hit ------BASE------");
                  vCR();
                  vLF();
                } else if (((0x41 <= din[i]) && (din[i] <= 0x55)) ||
                           (0x94 == din[i]) ||
                           ((0x81 <= din[i]) && (din[i] <= 0x93)))
                  if (din[i] != 0x80) {
                    if (irslot[i + 1] == din[i]) {
                      if (noinfo == 0)
                        ;
                      {
                        vCR();
                        vLF();
                      }
                      ++noinfo;
                      ChgColor(i);
                      asends("   ");
                      asendc(pl[i].playername, 10);
                      vChangeAttr(COLOR(WHT, BLK));
                      asends(" hit his own player");
                      vCR();
                      vLF();
                    } else {
                      if (pl[slotir[din[i]] - 1].used == 1) {
                        if (noinfo == 0) {
                          vCR();
                          vLF();
                        }
                        ++noinfo;
                        ChgColor(i);
                        asends("   ");
                        asendc(pl[i].playername, 10);
                        vChangeAttr(COLOR(WHT, BLK));
                        asends(" hit ");
                        ChgColor(slotir[din[i]] - 1);
                        asendc(pl[slotir[din[i]] - 1].playername, 10);
                        vCR();
                        vLF();
                      }
                    }
                  }
              }
            }
          }
          if (noinfo == 0) {
            vChangeAttr(COLOR(WHT, BLK));
            asends(".");
          }
          break;
      }
    }
  }
}

void setupslots() {
  irslot[1] = 0x42;
  irslot[2] = 0x43;
  irslot[3] = 0x44;
  irslot[4] = 0x45;
  irslot[5] = 0x46;
  irslot[6] = 0x47;
  irslot[7] = 0x48;
  irslot[8] = 0x49;
  irslot[9] = 0x4a;
  irslot[10] = 0x4b;
  irslot[11] = 0x4c;
  irslot[12] = 0x4d;
  irslot[13] = 0x4e;
  irslot[14] = 0x4f;
  irslot[15] = 0x50;
  irslot[16] = 0x51;
  irslot[17] = 0x52;
  irslot[18] = 0x53;
  irslot[19] = 0x54;
  irslot[20] = 0x55;
  irslot[21] = 0x94;
  irslot[22] = 0x81;
  irslot[23] = 0x82;
  irslot[24] = 0x83;
  irslot[25] = 0x84;
  irslot[26] = 0x85;
  irslot[27] = 0x86;
  irslot[28] = 0x87;
  irslot[29] = 0x88;
  irslot[30] = 0x89;
  irslot[31] = 0x8a;
  irslot[32] = 0x8b;
  irslot[33] = 0x8c;
  irslot[34] = 0x8d;
  irslot[35] = 0x8e;
  irslot[36] = 0x8f;
  irslot[37] = 0x90;
  irslot[38] = 0x91;
  irslot[39] = 0x92;
  irslot[40] = 0x93;
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

void HOST_recvsn(unsigned int port, char *buffer, size_t size) {
  while (size--) {
    do {
      ReadPort(port + 5, &pinfo);
    } while (pinfo.DR != 1);
    *buffer++ = inp(port);
  }
}
