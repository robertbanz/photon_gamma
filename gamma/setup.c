/**********
 setup.c
 **********/
#define SYSTEM_PROGRAM

#include <conio.h>
#include <dos.h>
#include "gamma.h"

void sys_config(byte configured) {
  char asd;
  char huge *add;
  int p, k, x, y;
  char j;
  curconfig.pcmode = newconfig.pcmode;
  curconfig.ps = newconfig.ps;
  curconfig.length = newconfig.length;
  curconfig.vidmode = newconfig.vidmode;
  curconfig.manuever = newconfig.manuever;
  curconfig.cdsel = newconfig.cdsel;
  curconfig.cdtype = newconfig.cdtype;
  curconfig.savedata = newconfig.savedata;
  curconfig.newscr = newconfig.newscr;

  curconfig.pcport = newconfig.pcport;
  curconfig.dcport = newconfig.dcport;
  curconfig.psport = newconfig.psport;
  curconfig.radioport = newconfig.radioport;
  curconfig.et1port = newconfig.et1port;
  curconfig.et2port = newconfig.et2port;
  curconfig.ecport = newconfig.ecport;

  if ((newconfig.pc == TRUE) &
      ((newconfig.pc != curconfig.pc) | (configured == FALSE))) {
    outp(PC, 0xE1); /*INIT CHAR FOR PC*/
    info("Init sent to PC...");
  }
  curconfig.pc = newconfig.pc;

  if ((newconfig.dc == TRUE) &
      ((newconfig.dc != curconfig.dc) | (configured == FALSE))) {
    outp(DC, 0xF5); /*INIT CHAR FOR DC*/
    info("Init sent to DC...");
  }
  curconfig.dc = newconfig.dc;
  curconfig.ec = newconfig.ec;

  if ((newconfig.tween == TEXT) &
      ((newconfig.tween != curconfig.tween) | (configured == FALSE))) {
    curconfig.tween = newconfig.tween;
    if (newconfig.vidmode == 0) {
      /* outp(0x3d9,0x11);*/ /*NOT !!! Set Blue Border*/
                             /* outp(0x3d8,0x28);*/
    }
    clrcga();
  }
  curconfig.pub_begin = newconfig.pub_begin;
  curconfig.lea_begin = newconfig.lea_begin;
  curconfig.ffa_begin = newconfig.ffa_begin;
  curconfig.pub_hcp = newconfig.pub_hcp;
  curconfig.lea_hcp = newconfig.lea_hcp;
  curconfig.ffa_hcp = newconfig.ffa_hcp;
  memcpy(curconfig.log_path, newconfig.log_path, 10);
  memcpy(curconfig.pod_path, newconfig.pod_path, 10);
  memcpy(curconfig.data_path, newconfig.data_path, 10);
  memcpy(curconfig.alpha_red, newconfig.alpha_red, 10);
  memcpy(curconfig.alpha_grn, newconfig.alpha_grn, 10);
  memcpy(curconfig.omega_grn, newconfig.omega_grn, 10);
  memcpy(curconfig.beta_red, newconfig.beta_red, 10);
  memcpy(curconfig.beta_grn, newconfig.beta_grn, 10);

  for (k = 0; k < 10; ++k) {
    if (curconfig.log_path[k] == 32) curconfig.log_path[k] = 0;
    if (curconfig.pod_path[k] == 32) curconfig.pod_path[k] = 0;
    if (curconfig.mess_path[k] == 32) curconfig.mess_path[k] = 0;
    if (curconfig.data_path[k] == 32) curconfig.data_path[k] = 0;
  }
}

/*compile these next two functions into one.*/

#pragma optimize("", off)

void ET_config1(byte configured) {
  struct com_6 status;
  byte j;
  int k;
  if ((newconfig.etfake != 1) & (newconfig.et1 != OFF) &
      ((configured == FALSE) | ((newconfig.et1 != curconfig.et1) |
                                (newconfig.field != curconfig.field) |
                                (newconfig.players != curconfig.players)))) {
    info("-ENTRY TERMINAL 1");
    ReadPort(ET1 + 6, &status);
    if (!status.CTS || !status.DSR) {
      printf("\007\007\007");
      info("POWER UP ENTRY TERMINAL #1 !");
      info("     or CTRL-Q to QUIT      ");
      do {
        if (kbhit())
          if (getch() == 17) FatalError("USER ABORTED CONFIGURATION");
        ReadPort(ET1 + 6, &status);
      } while (!status.CTS || !status.DSR);
      WaitPoll();
      WaitPoll();
      WaitPoll();
    }
    info("CONFIGURING TERMINAL");
    outp(ET1, ETINIT);
    WaitPoll();
    outp(ET1, 0xd2);
    WaitPoll();
    j = newconfig.players * 2;
    outp(ET1, j);
    k = 0;
    do {
      WaitPoll();
      ++k;
    } while (((inp(ET1)) != j) && (k < 3));
    if (k <= 3) {
      if (newconfig.field == 1)
        if (newconfig.et1 == RED)
          j = BETA_RED;
        else if (newconfig.et1 == GREEN)
          j = BETA_GRN;
        else if (newconfig.et1 == BOTH)
          j = BETA_BOTH_RED;
      if (newconfig.field == 2)
        if (newconfig.et1 == RED)
          j = ALPHA;
        else if (newconfig.et1 == GREEN)
          j = OMEGA;
        else if (newconfig.et1 == BOTH)
          j = ALPHA;
      k = 0;
      outp(ET1, j);
      do {
        WaitPoll();
        ++k;
      } while (((inp(ET1)) != j) && (k < 3));
      if (k <= 3) {
        outp(ET1, ETEND);
        WaitPoll();
        WaitPoll();
      } else
        FatalError("ET1 CONFIGURE ERROR 2");
    } else
      FatalError("ET1 CONFIGURE ERROR 1");
  }
}
void ET_config2(byte configured) {
  struct com_6 status;
  byte j;
  int k;
  if ((newconfig.etfake != 2) && (newconfig.et2 != OFF) &&
      ((configured == FALSE) || ((newconfig.et2 != curconfig.et2) ||
                                 (newconfig.field != curconfig.field) ||
                                 (newconfig.players != curconfig.players)))) {
    info("-ENTRY TERMINAL 2");
    ReadPort(ET2 + 6, &status);
    if (!status.DSR || !status.CTS) {
      printf("\007\007\007");
      info("POWER UP ENTRY TERMINAL #2 !");
      info("     or CTRL-Q to QUIT      ");
      do {
        if (kbhit())
          if (getch() == 17) FatalError("USER ABORTED CONFIGURATION");
        ReadPort(ET2 + 6, &status);
      } while (!status.DSR || !status.CTS);
      WaitPoll();
      WaitPoll();
      WaitPoll();
    }
    info("CONFIGURING TERMINAL");
    outp(ET2, ETINIT);
    WaitPoll();
    outp(ET2, 0xd2);
    WaitPoll();
    j = newconfig.players * 2;
    outp(ET2, j);
    k = 0;
    do {
      WaitPoll();
      ++k;
    } while (((inp(ET2)) != j) && (k < 3));
    if (k <= 3) {
      if (newconfig.field == 1)
        if (newconfig.et2 == RED)
          j = BETA_RED;
        else if (newconfig.et2 == GREEN)
          j = BETA_GRN;
        else if (newconfig.et2 == BOTH)
          j = BETA_BOTH_RED;
      if (newconfig.field == 2)
        if (newconfig.et2 == RED)
          j = ALPHA;
        else if (newconfig.et2 == GREEN)
          j = OMEGA;
        else if (newconfig.et2 == BOTH)
          j = ALPHA;
      k = 0;
      outp(ET2, j);
      do {
        WaitPoll();
        ++k;
      } while (((inp(ET2)) != j) && (k < 3));
      if (k <= 3)
        outp(ET2, ETEND);
      else
        FatalError("ET2 CONFIGURE ERROR 2");
    } else
      FatalError("ET2 CONFIGURE ERROR 1");
  }
}

#pragma optimize("", on)
void ConfigEts(byte configured) {
  ET_config1(configured);
  ET_config2(configured);
  curconfig.etfake = newconfig.etfake;
  curconfig.field = newconfig.field;
  curconfig.et1 = newconfig.et1;
  curconfig.et2 = newconfig.et2;
  curconfig.players = newconfig.players;
}

void FatalError(char *ts) {
  ResetIBM();
  printf("\n\n\n\007\007%s", ts);
  exit(-1);
}
