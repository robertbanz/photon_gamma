/*************
 newconfi.c
 *************/

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "asd.h"
#include "asdmenu.h"
#include "gamma.h"
#include "video.h"

int con_main(int);

void MonoPalette(void) {
  static struct mPalette mpala;
  struct mPalette *mpal;

  mpal = &mpala;
  mpal->borderstyle = 2;
  mpal->menuborder = hrcREV;
  mpal->menucolor = hrcNORM;
  mpal->menuselect = hrcNORM | hrcINTEN;
  mpal->menudim = hrcNORM;
  mpal->editborder = hrcREV;
  mpal->editcolor = hrcREV | hrcINTEN;
  mpal->yesnoborder = hrcREV;
  mpal->yesnocolor = hrcNORM;
  mpal->yesnoselect = hrcNORM | hrcINTEN;
  mpal->alertborder = hrcREV | hrcINTEN;
  mpal->alertcolor = hrcREV | hrcBLINK;
  mPushPalette(mpal);
}

void setupmenu(void) {
  char tempstring[13];
  FILE *fp;
  mAppendToMenu("Entry Terminals", 0, 0);
  mAppendToMenu("Progress Computer", 1, 0);
  mAppendToMenu("Data Computer", 2, 0);
  mAppendToMenu("Effects Computer", 3, 0);
  mAppendToMenu("Phasor Stations", 4, 0);
  mAppendToMenu("CD Track Selection", 5, 0);
  mAppendToMenu("Field Type", 6, 0);
  mAppendToMenu("Number of Players", 7, 0);
  mAppendToMenu("New Screen Level", 8, 0);
  mAppendToMenu("Beginning Scores", 9, 0);
  mAppendToMenu("Handicapp Mode", 10, 0);
  mAppendToMenu("Default Team Names", 11, 0);
  mAppendToMenu("Game Length", 12, 0);
  mAppendToMenu("Video Mode [test]", 13, 0);
  mAppendToMenu("Message File path", 14, 0);
  mAppendToMenu("Database path", 15, 0);
  mAppendToMenu("Log File path", 16, 0);
  mAppendToMenu("Pod Performance Path", 17, 0);
  mAppendToMenu("CD Player Interface Type", 18, 0);
  mAppendToMenu("Phantom ET [test]", 19, 0);
  mAppendToMenu("PC Mode", 20, 0);
  mAppendToMenu("Game Data Save", 21, 0);
  mAppendToMenu("Save and QUIT", 80, 0);
  mPopUpMenu(40, 0, con_main, 0, 0);
}

int con_hcp(int sel) {
  byte *k;
  mPushMarker();
  mAppendToMenu("Handicapp OFF", 0, 0);
  mAppendToMenu("Handicapp ON", 1, 0);
  switch (sel) {
    case 0:
      k = &newconfig.pub_hcp;
      break;
    case 1:
      k = &newconfig.lea_hcp;
      break;
    case 2:
      k = &newconfig.ffa_hcp;
      break;
  }
  *k = (byte)mPopUpMenu(20, 12 + sel, NOESC, *k, 0);
}
int con_scores(int sel) {
  int *k;
  mPushMarker();
  mAppendToMenu("     0", 0, 0);
  mAppendToMenu("  1000", 1000, 0);
  mAppendToMenu("Custom", 1, 0);
  switch (sel) {
    case 0:
      k = &newconfig.pub_begin;
      break;
    case 1:
      k = &newconfig.lea_begin;
      break;
    case 2:
      k = &newconfig.ffa_begin;
      break;
  }
  *k = mPopUpMenu(20, 12 + sel, NOESC, ((*k == 0 ? 0 : (*k == 1000 ? 1 : (2)))),
                  0);
}

int con_et(int sel) {
  mPushMarker();
  mAppendToMenu("Off", 0, 0);
  if (newconfig.field == 1) {
    mAppendToMenu("Red Team", 1, 0);
    mAppendToMenu("Green Team", 2, 0);
    mAppendToMenu("Both Teams", 4, 0);
  } else {
    mAppendToMenu("Alpha Field", 1, 0);
    mAppendToMenu("Omega Field", 2, 0);
  }
  if (sel == 1) {
    newconfig.et1 =
        mPopUpMenu(20, 3, NOESC, newconfig.et1 == 4 ? 3 : newconfig.et1, 0);
  } else if (sel == 2) {
    newconfig.et2 =
        mPopUpMenu(20, 4, NOESC, newconfig.et2 == 4 ? 3 : newconfig.et2, 0);
  }
}

int con_names(int sel) {
  switch (sel) {
    case 0:
      mEditString(newconfig.beta_red, "Single Red Team: ", 10, 20, 15);
      mEditString(newconfig.beta_grn, "Single Green Team: ", 10, 20, 15);
      break;
    case 1:
      mEditString(newconfig.alpha_red, "First Red Team: ", 10, 20, 16);
      mEditString(newconfig.alpha_grn, "First Green Team: ", 10, 20, 16);
      break;
    case 2:
      mEditString(newconfig.omega_red, "Second Red Team: ", 10, 20, 17);
      mEditString(newconfig.omega_grn, "Second Green Team: ", 10, 20, 17);
      break;
  }
}

int con_main(int sel) {
  FILE *fp;
  int rval, flag;
  switch (sel) {
    case 0:
      mPushMarker();
      mAppendToMenu("Entry Terminal 1", 1, 0);
      mAppendToMenu("Entry Terminal 2", 2, 0);
      rval = mPopUpMenu(15, 2, con_et, 0, 0);
      break;
    case 1:
      mPushMarker();
      mAppendToMenu("Progress Computer OFF", OFF, 0);
      mAppendToMenu("Progress Computer ON", ON, 0);
      newconfig.pc = (byte)mPopUpMenu(15, 3, NOESC, newconfig.pc, 0);
      break;
    case 2:
      mPushMarker();
      mAppendToMenu("Data Computer OFF", OFF, 0);
      mAppendToMenu("Data Computer ON", ON, 0);
      newconfig.dc = (byte)mPopUpMenu(15, 4, NOESC, newconfig.dc, 0);
      break;
    case 3:
      mPushMarker();
      mAppendToMenu("Effects Computer OFF", OFF, 0);
      mAppendToMenu("Effects Computer ON", ON, 0);
      newconfig.ec = (byte)mPopUpMenu(15, 5, NOESC, newconfig.ec, 0);
      break;
    case 4:
      mPushMarker();
      mAppendToMenu("Phasor Stations OFF", OFF, 0);
      mAppendToMenu("Phasor Stations ON", ON, 0);
      newconfig.ps = (byte)mPopUpMenu(15, 6, NOESC, newconfig.ps, 0);
      break;
    case 5:
      mPushMarker();
      mAppendToMenu("RANDOM selection", 1, 0);
      mAppendToMenu("1-5 ORDERED selection", 2, 0);
      newconfig.cdsel =
          (byte)mPopUpMenu(15, 7, NOESC, (newconfig.cdsel - 1), 0);
      break;
    case 6:
      mPushMarker();
      mAppendToMenu("BETA Single Field", 1, 0);
      mAppendToMenu("ALPHA/OMEGA Dual Field", 2, 0);
      newconfig.field =
          (byte)mPopUpMenu(15, 8, NOESC, (newconfig.field - 1), 0);
      break;
    case 7:
      mPushMarker();
      mAppendToMenu("8 (16 in game)", 8, 0);
      mAppendToMenu("9 (18 in game)", 9, 0);
      mAppendToMenu("10 (20 in game)", 10, 0);
      if (newconfig.field != 2) {
        mAppendToMenu("11 (22 in game)  SINGLE ONLY !", 11, 0);
        mAppendToMenu("12 (24 in game)  SINGLE ONLY !", 12, 0);
        mAppendToMenu("13 (26 in game)  SINGLE ONLY !", 13, 0);
        mAppendToMenu("14 (28 in game)  SINGLE ONLY !", 14, 0);
        mAppendToMenu("15 (30 in game)  SINGLE ONLY !", 15, 0);
        mAppendToMenu("20 (40 in game)  [Test Version]", 20, 0);
      }
      newconfig.players =
          (byte)mPopUpMenu(15, 9, NOESC, (newconfig.players - 8), 0);
      break;
    case 8:
      mPushMarker();
      mAppendToMenu("Never", 0, 0);
      mAppendToMenu("League & FFA Only", 1, 0);
      mAppendToMenu("All modes", 2, 0);
      newconfig.newscr = (byte)mPopUpMenu(15, 10, NOESC, (newconfig.newscr), 0);
      break;
    case 9:
      mPushMarker();
      mAppendToMenu("Public", 0, 0);
      mAppendToMenu("League", 1, 0);
      mAppendToMenu("Free For All", 2, 0);
      rval = mPopUpMenu(15, 11, con_scores, 0, 0);
      break;
    case 10:
      mPushMarker();
      mAppendToMenu("Public", 0, 0);
      mAppendToMenu("League", 1, 0);
      mAppendToMenu("Free For All", 2, 0);
      rval = mPopUpMenu(15, 12, con_hcp, 0, 0);
      break;
    case 11:
      mPushMarker();
      mAppendToMenu("Singe Field", 0, 0);
      mAppendToMenu("Dual Field #1", 1, 0);
      mAppendToMenu("Dual Field #2", 2, 0);
      rval = mPopUpMenu(15, 13, con_names, 0, 0);
      break;
    case 12:
      mPushMarker();
      mAppendToMenu("6 Minutes", 360, 0);
      mAppendToMenu("2 Minutes", 120, 0);
      mAppendToMenu("10 Seconds", 10, 0);
      mAppendToMenu("15 Minutes", (15 * 60), 0);
      mAppendToMenu("Custom Time", 1, 0);
      newconfig.length = mPopUpMenu(
          15, 14, NOESC,
          (newconfig.length == 360
               ? 0
               : (newconfig.length == 120 ? 1
                                          : (newconfig.length == 10 ? 2 : 3))),
          0);
      break;
    case 13:
      mPushMarker();
      mAppendToMenu("CGA (40 col)", 0, 0);
      mAppendToMenu("EGA/VGA (80 col)", 1, 0);
      newconfig.vidmode =
          (byte)mPopUpMenu(15, 15, NOESC, (int)newconfig.vidmode, 0);
      break;
    case 14:
      mEditString(newconfig.mess_path, "Enter Message File Path: ", 10, 15, 16);
      break;
    case 15:
      mEditString(newconfig.data_path, "Enter Database File Path: ", 10, 15,
                  17);
      break;
    case 16:
      mEditString(newconfig.log_path, "Enter Log File Path: ", 10, 15, 18);
      break;
    case 17:
      mEditString(newconfig.pod_path, "Enter Pod Perfomance Path: ", 10, 15,
                  19);
      break;
    case 18:
      mPushMarker();
      mAppendToMenu("LPT 0 (Mono Card)", 0x3BC, 0);
      mAppendToMenu("LPT 1 (Normal)", 0x378, 0);
      mAppendToMenu("SLAVE in Game Computer", 0x200, 0);
      mAppendToMenu("SLAVE in Post Game Displayer", 1, 0);
      newconfig.cdtype =
          mPopUpMenu(15, 15, NOESC,
                     (newconfig.cdtype == 0x3BC
                          ? 0
                          : (newconfig.cdtype == 0x378
                                 ? 1
                                 : (newconfig.cdtype == 0x200 ? 2 : 3))),
                     0);
      break;
    case 19:
      mPushMarker();
      mAppendToMenu("None", 0, 0);
      mAppendToMenu("Entry Terminal 1", 1, 0);
      mAppendToMenu("Entry Terminal 2", 2, 0);
      newconfig.etfake = mPopUpMenu(15, 15, NOESC, newconfig.etfake, 0);
      break;
    case 20:
      mPushMarker();
      mAppendToMenu("Don't Save", 0, 0);
      mAppendToMenu("Save League ONLY", 1, 0);
      mAppendToMenu("Save ALL", 2, 0);
      newconfig.pcmode = mPopUpMenu(15, 15, NOESC, newconfig.pcmode, 0);
      break;
    case 21:
      mPushMarker();
      mAppendToMenu("Don't Save Game Data", 0, 0);
      mAppendToMenu("Save Game Data", 1, 0);
      newconfig.savedata = mPopUpMenu(15, 15, NOESC, newconfig.savedata, 0);
      break;
    case pESC:
    case 80:
      if ((newconfig.et1 == newconfig.et2) ||
          ((newconfig.field == 2) &&
           ((newconfig.et1 == BOTH) || (newconfig.et2 == BOTH))) ||
          ((newconfig.et1 == BOTH) && (newconfig.et2 != OFF)) ||
          ((newconfig.et2 == BOTH) && (newconfig.et1 != OFF)))
        mAlert("ET CONFIGURATONS ARE INCORRECT !!!", 20, 10);
      else {
        if (mYesNo("Save these defaults ?", 15, 19, 0)) {
          if ((fp = fopen("system.ini", "wb")) != NULL) {
            fwrite(&newconfig, sizeof(struct config), 1, fp);
            fclose(fp);
          } else {
            mAlert("DISK ERROR: CONFIGURAION NOT SAVED !", 20, 20);
          }
        }
        return (pESC);
      }
      break;
    case 81:
      return (pESC);
  }
  return (mLOOP);
}

