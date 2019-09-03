/*************
  CD.C
**************/
#define SYSTEM_PROGRAM

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cdromf.h"
#include "gamma.h"

void CD_track(byte track) { CDROMSetTrack(track); }

void CD_play(void) { CDROMResume(); }

void CD_stop(void) { CDROMStop(); }

void CD_pause(void) { CDROMStop(); }

void CD_reset(void) {
  if (!CDROMSetup()) {
    ResetIBM();
    exit(-1);
  }
}
