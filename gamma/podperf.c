/**********
  podperf.c
  *********/

#define SYSTEM_PROGRAM
#include <dos.h>
#include <time.h>
#include "gamma.h"

void SetupPodId(void) {
  idsig[1] = 0x60;
  idsig[2] = 0x61;
  idsig[3] = 0x62;
  idsig[4] = 0x63;
  idsig[5] = 0x64;
  idsig[6] = 0x65;
  idsig[7] = 0x66;
  idsig[8] = 0x67;
  idsig[9] = 0x68;
  idsig[10] = 0x69;
  idsig[11] = 0x70;
  idsig[12] = 0x71;
  idsig[13] = 0x72;
  idsig[14] = 0x73;
  idsig[15] = 0x74;
  idsig[16] = 0x75;
  idsig[17] = 0x76;
  idsig[18] = 0x77;
  idsig[19] = 0x78;
  idsig[20] = 0x79;
  idsig[21] = 0x6a;
  idsig[22] = 0x6b;
  idsig[23] = 0x6c;
  idsig[24] = 0x6d;
  idsig[25] = 0x6e;
  idsig[26] = 0x6f;
  idsig[27] = 0x7a;
  idsig[28] = 0x7b;
  idsig[29] = 0x7c;
  idsig[30] = 0x7d;
  idsig[31] = 0x7e;
  idsig[32] = 0x7f;
  idsig[33] = 0x5f;
  idsig[34] = 0x5d;
  idsig[35] = 0x01;
  idsig[36] = 0x02;
  idsig[37] = 0x03;
  idsig[38] = 0x04;
  idsig[39] = 0x05;
  idsig[40] = 0x06;
  idsig[41] = 0x07;
  idsig[42] = 0x08;
  idsig[43] = 0x09;
  idsig[44] = 0x0a;

  sigid[0x60] = 1;
  sigid[0x61] = 2;
  sigid[0x62] = 3;
  sigid[0x63] = 4;
  sigid[0x64] = 5;
  sigid[0x65] = 6;
  sigid[0x66] = 7;
  sigid[0x67] = 8;
  sigid[0x68] = 9;
  sigid[0x69] = 10;
  sigid[0x70] = 11;
  sigid[0x71] = 12;
  sigid[0x72] = 13;
  sigid[0x73] = 14;
  sigid[0x74] = 15;
  sigid[0x75] = 16;
  sigid[0x76] = 17;
  sigid[0x77] = 18;
  sigid[0x78] = 19;
  sigid[0x79] = 20;
  sigid[0x6a] = 21;
  sigid[0x6b] = 22;
  sigid[0x6c] = 23;
  sigid[0x6d] = 24;
  sigid[0x6e] = 25;
  sigid[0x6f] = 26;
  sigid[0x7a] = 27;
  sigid[0x7b] = 28;
  sigid[0x7c] = 29;
  sigid[0x7d] = 30;
  sigid[0x7e] = 31;
  sigid[0x7f] = 32;
  sigid[0x5f] = 33;
  sigid[0x5d] = 34;
  sigid[0x01] = 35;
  sigid[0x02] = 36;
  sigid[0x03] = 37;
  sigid[0x04] = 38;
  sigid[0x05] = 39;
  sigid[0x06] = 40;
  sigid[0x07] = 41;
  sigid[0x08] = 42;
  sigid[0x09] = 43;
  sigid[0x0a] = 44;
}

byte IsId(byte info) {
  if ((0x5f <= info) && (info <= 0x7f))
    return (TRUE);
  else if (info == 0x5d)
    return (TRUE);
  else if ((0x01 <= info) && (info <= 0x0a))
    return (TRUE);
  else
    return (FALSE);
}
