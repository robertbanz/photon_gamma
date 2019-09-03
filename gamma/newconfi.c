/*************
 newconfi.c
 *************/
#define SYSTEM_PROGRAM

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "asd.h"
#include "asdmenu.h"
#include "gamma.h"
#undef RED
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
  mPushMarker();
  mAppendToMenu("GAME CONFIGURATION", 0, 0);
  mAppendToMenu("PERHIPHERAL CONFIGURATION", 1, 0);
  mAppendToMenu("Video Mode", 2, 0);
  mAppendToMenu("Bad Stuff on Screen", 3, 0);
  mAppendToMenu("Save Score Data", 4, 0);
  mAppendToMenu("Save Radio Data", 5, 0);
  mAppendToMenu("PATHS", 6, 0);
  mAppendToMenu("ET Test Mode", 7, 0);
  mAppendToMenu("Save and QUIT", 80, 0);
  mPopUpMenu(40, 0, con_main, 0, 0);
}

int con_hcp(int sel) {
  byte *k;
  if (sel == pESC) return pESC;
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
  return 0;
}

int con_scores(int sel) {
  int *k;
  if (sel == pESC) return pESC;
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
  *k = mPopUpMenu(20, 12 + sel, NOESC, *k == 0 ? 0 : *k == 1000 ? 1 : 2, 0);
  return 0;
}

int PortSelection(int o) {
  int rval;
  mPushMarker();
  mAppendToMenu(" COM 1  (0x3f8)", 0x3f8, 0);
  mAppendToMenu(" COM 2  (0x2f8)", 0x2f8, 0);
  mAppendToMenu(" COM 3  (0x3e8)", 0x3e8, 0);
  mAppendToMenu(" COM 4  (0x2e8)", 0x2e8, 0);
  mAppendToMenu("HOST 1  (0x240)", 0x240, 0);
  mAppendToMenu("HOST 2  (0x248)", 0x248, 0);
  mAppendToMenu("HOST 3  (0x250)", 0x250, 0);
  mAppendToMenu("HOST 4  (0x258)", 0x258, 0);
  mAppendToMenu("HOST 5  (0x260)", 0x260, 0);
  mAppendToMenu("HOST 6  (0x268)", 0x268, 0);
  mAppendToMenu("HOST 7  (0x270)", 0x270, 0);
  mAppendToMenu("HOST 8  (0x278)", 0x278, 0);
  mAppendToMenu("INACTIVE", 0, 0);
  rval = mPopUpMenu(
      20, 4, 0,
      (o == 0x3f8)
          ? 0
          : (o == 0x2f8)
                ? 1
                : (o == 0x3e8)
                      ? 2
                      : (o == 0x2e8)
                            ? 3
                            : (o == 0x240)
                                  ? 4
                                  : (o == 0x248)
                                        ? 5
                                        : (o == 0x250)
                                              ? 6
                                              : (o == 0x258)
                                                    ? 7
                                                    : (o == 0x260)
                                                          ? 8
                                                          : (o == 0x268)
                                                                ? 9
                                                                : (o == 0x270)
                                                                      ? 10
                                                                      : (o ==
                                                                         0x278)
                                                                            ? 11
                                                                            : (o ==
                                                                               0)
                                                                                  ? 12
                                                                                  : 12,
      0);

  return rval;
}

int con_et(int sel) {
  if (sel == pESC) return pESC;
  if (sel == 2) {
    newconfig.et1port = PortSelection(newconfig.et1port);
    if (!newconfig.et1port) newconfig.et1 = 0;
  } else if (sel == 3) {
    newconfig.et2port = PortSelection(newconfig.et2port);
    if (!newconfig.et2port) newconfig.et2 = 0;
  } else {
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
    if (sel == 0) {
      newconfig.et1 =
          mPopUpMenu(20, 3, NOESC, newconfig.et1 == 4 ? 3 : newconfig.et1, 0);
    } else if (sel == 1) {
      newconfig.et2 =
          mPopUpMenu(20, 4, NOESC, newconfig.et2 == 4 ? 3 : newconfig.et2, 0);
    }
  }
  return 0;
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
    case pESC:
      return pESC;
  }
  return 0;
}

int con_game(int sel) {
  int rval;

  switch (sel) {
    case 0: /*field type*/
      mPushMarker();
      mAppendToMenu("Single Field", 1, 0);
      mAppendToMenu("Dual Field", 2, 0);
      newconfig.field =
          (byte)mPopUpMenu(15, 8, NOESC, (newconfig.field - 1), 0);
      break;
    case 1: /*Number of Players*/
      mPushMarker();
      mAppendToMenu("8 (16 in game)", 8, 0);
      mAppendToMenu("9 (18 in game)", 9, 0);
      mAppendToMenu("10 (20 in game)", 10, 0);
      if (newconfig.field != 2) {
        mAppendToMenu("11 (22 in game)", 11, 0);
        mAppendToMenu("12 (24 in game)", 12, 0);
        mAppendToMenu("13 (26 in game)", 13, 0);
        mAppendToMenu("14 (28 in game)", 14, 0);
        mAppendToMenu("15 (30 in game)", 15, 0);
        mAppendToMenu("20 (40 in game)  [Test Mode]", 20, 0);
      }
      newconfig.players =
          (byte)mPopUpMenu(15, 9, NOESC, (newconfig.players - 8), 0);
      break;
    case 2: /* Beginning Scores*/

      mPushMarker();
      mAppendToMenu("Public", 0, 0);
      mAppendToMenu("League", 1, 0);
      mAppendToMenu("Free For All", 2, 0);
      rval = mPopUpMenu(15, 11, con_scores, 0, 0);
      if (rval == pESC) return pESC;
      break;
    case 3: /*Handicapp Levels*/
      while (1) {
        mPushMarker();
        mAppendToMenu("Public", 0, 0);
        mAppendToMenu("League", 1, 0);
        mAppendToMenu("Free For All", 2, 0);
        rval = mPopUpMenu(15, 12, con_hcp, 0, 0);
        if (rval == pESC) break;
      }
      break;
    case 4: /*Game Length*/
      mPushMarker();
      mAppendToMenu("6 Minutes", 360, 0);
      mAppendToMenu("2 Minutes", 120, 0);
      mAppendToMenu("10 Seconds (super quick test mode)", 10, 0);
      mAppendToMenu("15 Minutes", (15 * 60), 0);
      mAppendToMenu("12 Minutes 45 Seconds (doubler)", (12 * 60) + 45, 0);
      /*mAppendToMenu("Custom Time",1,0);*/
      newconfig.length = mPopUpMenu(
          15, 14, NOESC,
          (newconfig.length == 360)
              ? 0
              : (newconfig.length == 120)
                    ? 1
                    : (newconfig.length == 10)
                          ? 2
                          : (newconfig.length == (15 * 60))
                                ? 3
                                : (newconfig.length == (12 * 60) + 45) ? 4 : 5,
          0);

      break;
    case 5: /*default team names*/
      while (1) {
        mPushMarker();
        mAppendToMenu("Singe Field", 0, 0);
        mAppendToMenu("Dual Field #1", 1, 0);
        mAppendToMenu("Dual Field #2", 2, 0);
        rval = mPopUpMenu(15, 13, con_names, 0, 0);
        if (rval == pESC) break;
      }
      break;
    case pESC:
      return pESC;
  }
  return 0;
}

int con_perh(int sel) {
  int rval;

  switch (sel) {
    case 0:
    case 1: /*entry terminal*/
      while (1) {
        mPushMarker();
        mAppendToMenu("Terminal Mode", sel, 0);
        mAppendToMenu("IO Port", 2 + sel, 0);
        rval = mPopUpMenu(15, 12, con_et, 0, 0);
        if (rval == pESC) break;
      }
      break;
    case 2:
      newconfig.pcport = PortSelection(newconfig.pcport);
      if (!newconfig.pcport)
        newconfig.pc = 0;
      else
        newconfig.pc = 1;
      break;
    case 3:
      newconfig.dcport = PortSelection(newconfig.dcport);
      if (!newconfig.dcport)
        newconfig.dc = 0;
      else
        newconfig.dc = 1;
      break;
    case 4:
      newconfig.ecport = PortSelection(newconfig.ecport);
      if (!newconfig.ecport)
        newconfig.ec = 0;
      else
        newconfig.ec = 1;
      break;
    case 5:
      newconfig.psport = PortSelection(newconfig.psport);
      if (!newconfig.psport)
        newconfig.ps = 0;
      else
        newconfig.ps = 1;
      break;
    case 6:
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
    case 7:
      newconfig.radioport = PortSelection(newconfig.radioport);
      break;
    case pESC:
      return pESC;
  }
  return 0;
}

int con_paths(int sel) {
  switch (sel) {
    case 1:
      mEditString(newconfig.mess_path, "Enter Message File Path: ", 10, 15, 16);
      break;
    case 2:
      mEditString(newconfig.data_path, "Enter Database File Path: ", 10, 15,
                  17);
      break;
    case 3:
      mEditString(newconfig.log_path, "Enter Log File Path: ", 10, 15, 18);
      break;
    case 4:
      mEditString(newconfig.pod_path, "Enter Pod Perfomance Path: ", 10, 15,
                  19);
      break;
    case pESC:
      return pESC;
  }
  return 0;
}
int con_main(int sel) {
  FILE *fp;
  int rval, flag;
  switch (sel) {
    case 0:
      while (1) {
        mPushMarker();
        mAppendToMenu("Field Type", 0, 0);
        mAppendToMenu("Number of Players", 1, 0);
        mAppendToMenu("Beginning Scores", 2, 0);
        mAppendToMenu("Handicapp Levels", 3, 0);
        mAppendToMenu("Game Length", 4, 0);
        mAppendToMenu("Default Team names", 5, 0);
        rval = mPopUpMenu(15, 2, con_game, 0, 0);
        if (rval == pESC) break;
      }
      break;
    case 1:
      while (1) {
        mPushMarker();
        mAppendToMenu("Entry Terminal 1", 0, 0);
        mAppendToMenu("Entry Terminal 2", 1, 0);
        mAppendToMenu("Hit By Hit Display", 2, 0);
        mAppendToMenu("Previous Game Display", 3, 0);
        mAppendToMenu("Effects Computer", 4, 0);
        mAppendToMenu("Phasor Stations", 5, 0);
        mAppendToMenu("CD Player", 6, 0);
        mAppendToMenu("Central Radio", 7, 0);
        rval = mPopUpMenu(15, 2, con_perh, 0, 0);
        if (rval == pESC) break;
      }
      break;
    case 2:
      mPushMarker();
      mAppendToMenu("CGA (40 col)", 0, 0);
      mAppendToMenu("EGA/VGA (80 col)", 1, 0);
      newconfig.vidmode =
          (byte)mPopUpMenu(15, 15, NOESC, (int)newconfig.vidmode, 0);
      break;
    case 3:
      mPushMarker();
      mAppendToMenu("Never Used", 0, 0);
      mAppendToMenu("League & FFA Only", 1, 0);
      mAppendToMenu("All Games", 2, 0);
      newconfig.newscr = (byte)mPopUpMenu(15, 10, NOESC, (newconfig.newscr), 0);
      break;
    case 4:
      mPushMarker();
      mAppendToMenu("Don't Save Game Scores", 0, 0);
      mAppendToMenu("Save Game Scores", 1, 0);
      newconfig.savedata = mPopUpMenu(15, 15, NOESC, newconfig.savedata, 0);
      break;
    case 5:
      mPushMarker();
      mAppendToMenu("Don't Save", 0, 0);
      mAppendToMenu("Save League ONLY", 1, 0);
      mAppendToMenu("Save ALL", 2, 0);
      newconfig.pcmode = mPopUpMenu(15, 15, NOESC, newconfig.pcmode, 0);
      break;
    case 6:
      while (1) {
        mPushMarker();
        mAppendToMenu("Messages", 0, 0);
        mAppendToMenu("Game Data", 1, 0);
        mAppendToMenu("Log File", 2, 0);
        mAppendToMenu("Player Unit Performance", 3, 0);
        rval = mPopUpMenu(15, 2, con_paths, 0, 0);
        if (rval == pESC) break;
      }
      break;
    case 7:
      mPushMarker();
      mAppendToMenu("None", 0, 0);
      mAppendToMenu("Entry Terminal 1", 1, 0);
      mAppendToMenu("Entry Terminal 2", 2, 0);
      newconfig.etfake = mPopUpMenu(15, 15, NOESC, newconfig.etfake, 0);
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
