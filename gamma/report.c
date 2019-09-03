#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned char byte;
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

struct playrinfo {
  char playername[10];
  int hitopp[40];
  char baseflag;
  char used;
  char reset;
} pl[40];

#pragma pack(1)
struct gamerec {
  struct dostime_t curtime;
  struct dosdate_t curdate;
  struct {
    int score;
    struct podstruct pod;
  } sp[40];
  struct gamestruct game;
  struct playrinfo player[40];
  byte GameData[360][40];
} rec;

#pragma pack()
unsigned char irslot[256];
unsigned char slotir[256];
void setupslots(void);
void Performance(int min, int max);

struct player {
  char reset;
  char base;
} pls[40];

void putsn(char *ts, int num) {
  while (num--) putchar(*ts++);
}

void printaction(int time, int slot) {
  if (rec.player[slot].used) {
    if ((pls[slot].reset == 0) && (rec.GameData[time][slot] == 0xEE)) {
      printf("%3d:  ", time);
      putsn(rec.player[slot].playername, 10);
      printf("  reset\n");
      pls[slot].reset = 1;
    }

    else if ((pls[slot].base == 0) && ((rec.GameData[time][slot] == 0x35) ||
                                       (rec.GameData[time][slot] == 0x2b))) {
      printf("%3d:  ", time);
      putsn(rec.player[slot].playername, 10);
      printf("  hit base\n");
      pls[slot].base = 1;
    } else if (((0x41 <= rec.GameData[time][slot]) &&
                (rec.GameData[time][slot] <= 0x55)) ||
               (0x94 == rec.GameData[time][slot]) ||
               ((0x81 <= rec.GameData[time][slot]) &&
                (rec.GameData[time][slot] <= 0x93)))
      if (rec.GameData[time][slot] != 0x80) {
        {
          if (irslot[slot + 1] == rec.GameData[time][slot]) {
            printf("%3d:  ", time);
            putsn(rec.player[slot].playername, 10);
            printf("  hit his own player\n");
          } else {
            if (rec.player[slotir[rec.GameData[time][slot]] - 1].used) {
              printf("%3d:  ", time);
              putsn(rec.player[slot].playername, 10);
              printf("  hit  ");
              putsn(rec.player[slotir[rec.GameData[time][slot]] - 1].playername,
                    10);
              printf("\n");
            }
          }
        }
      }
  }
}

main(int argc, char *argv[]) {
  char ts[80];
  int i, j;
  FILE *fp;
  fp = fopen(argv[1], "rb");
  setupslots();
  fread(&rec, sizeof(struct gamerec), 1, fp);
  printf("File: %s\n", argv[1]);
  printf("Game Number: %3d, %2d:%2d   %2d/%2d/%4d\n", rec.game.number,
         rec.curtime.hour, rec.curtime.minute, rec.curdate.month,
         rec.curdate.day, rec.curdate.year);
  printf("------------------------------------\n");
  putsn(rec.game.redtm1, 10);
  putsn(" vs ", 4);
  putsn(rec.game.grntm1, 10);
  printf("\n\n\n");
  for (i = 0; i < 360; ++i) {
    for (j = 0; j < 40; ++j) printaction(i, j);
  }
  j = 0;
  printf("\f\n\n\nRED TEAM INFO:\n");
  printf("           score    pod id\n");
  for (i = 0; i < 20; ++i) {
    if (rec.player[i].used == 1) {
      putsn(rec.player[i].playername, 10);
      printf("%5d   %2d\n", rec.sp[i].score, rec.sp[i].pod.podid);
      j += rec.sp[i].score;
    }
  }
  printf("Total Score:  %6d\n", j);
  j = 0;
  printf("\f\n\n\nGREEN TEAM INFO:\n");
  printf("           score    pod id\n");
  for (i = 20; i < 39; ++i) {
    if (rec.player[i].used == 1) {
      putsn(rec.player[i].playername, 10);
      printf("%5d     %2d\n", rec.sp[i].score, rec.sp[i].pod.podid);
      j += rec.sp[i].score;
    }
  }
  printf("Total Score:  %6d\n", j);
  printf("\f\n\n\nRED TEAM POD PERFORMANCE\n");
  Performance(0, 19);
  printf("\f\n\n\nGREEN TEAM POD PERFORMANCE\n");
  Performance(20, 39);
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

void Performance(int min, int max) {
  int i;

  printf("                               POD TX/      CRAD TX/\n");
  printf("           AVG GAME ACTION     CRAD RX      POD RX\n");
  printf("           ---------------     ---------    ---------\n");
  printf(
      "POD  slot   ID  BASE   VAL     BAD   NO     BAD   NO     ---HITS---\n");
  printf(
      " #     #        CODE  CODE     CODE  COM    CODE  COM    SENT  ACKN   "
      "RESET\n");
  printf(
      "------------------------------------------------------------------------"
      "---\n\n\n");
  for (i = min; i <= max; ++i)
    if (rec.player[i].used == 1)
      printf(
          "%3d   %3d  %3d   %3d   %3d      %3d  %3d     %3d  %3d     %3d   %3d "
          "   %3d\n",
          rec.sp[i].pod.podid, i + 1, rec.sp[i].pod.id, rec.sp[i].pod.base,
          rec.sp[i].pod.valcode, rec.sp[i].pod.txbad, rec.sp[i].pod.txmiss,
          rec.sp[i].pod.rxbad, rec.sp[i].pod.rxmiss, rec.sp[i].pod.hitsent,
          rec.sp[i].pod.hitackn, pls[i].reset);
}
