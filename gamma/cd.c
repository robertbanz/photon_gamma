/*************
  CD.C
**************/
#define SYSTEM_PROGRAM

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamma.h"

#define LPT1 0x3BC
#define LPT2 0x378

#define CDPORT LPT1

#define INHIBIT 0x20
#define CDDELAY 20

#define PLAY 0x1B
#define STOP 0x1C
#define PAUSE 0x1D
#define CLEAR 0x1E
#define TIME 0x1F

#pragma optimize("", off)
void CD_sys(byte inf) {
  int cdi, cdj;
  if (curconfig.cdtype == 1) {
    charout(DC, 0xE6); /*DISABLE PRINTER*/
    if (WaitAck(DC, 1) != 0) info("! DC TIMEOUT ON FUNC. 0xE5/CD_sys");
    charout(DC, 0xF7);
    if (WaitAck(DC, 1) != 0) info("! DC TIMEOUT ON FUNC. 0xF7/CD_sys");
    charout(DC, inf);
    if (WaitAck(DC, 1) != 0) info("! DC TIMEOUT ON FUNC. CD TRK/CD_sys");
    cdi = slot;
    for (cdj = 0; cdj < CDDELAY; ++cdj) {
      while (slot == cdi)
        ;
      cdi = slot;
    }
    charout(DC, 0xF7);
    if (WaitAck(DC, 1) != 0) info("! DC TIMEOUT ON FUNC 0xF2 #2/CD_sys");
    charout(DC, INHIBIT);
    if (WaitAck(DC, 1) != 0) info("! DC TIMEOUT ON FUNC CD INH /CD_sys");
    for (cdj = 0; cdj < 40; ++cdj) {
      while (slot == cdi)
        ;
      cdi = slot;
    }
    charout(DC, 0xE7); /*ENABLE PRINTER*/
    if (WaitAck(DC, 1) != 0) info("! DC TIMEOUT ON FUNC 0xE7/CD_sys");
  } else {
    outp(curconfig.cdtype, inf);
    for (cdj = 0; cdj < CDDELAY; ++cdj) {
      while (slot == cdi)
        ;
      cdi = slot;
    }
    outp(curconfig.cdtype, INHIBIT);
    for (cdj = 0; cdj < CDDELAY; ++cdj) {
      while (slot == cdi)
        ;
      cdi = slot;
    }
  }
}
#pragma optimize("", on)

void CD_track(byte track) { CD_sys((byte)(track + 16)); }

void CD_play(void) { CD_sys(PLAY); }

void CD_stop(void) { CD_sys(STOP); }

void CD_pause(void) { CD_sys(PAUSE); }

void CD_clear(void) { CD_sys(CLEAR); }

void CD_time(void) { CD_sys(TIME); }
