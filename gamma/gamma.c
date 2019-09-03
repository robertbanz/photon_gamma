/********************/
/*  GAMMA.C	    */
/********************/
/*#define DEBUG*/
#define SYSTEM_PROGRAM
/* 060692  Fixed problems with people getting base points in freeforall */
/* 		  (hey, it only happened in Blowcago with their sucky radio!
 */
/* 		  Fixed problems with GC->PGD xfer locking up.	shouldn't
 * happen */
/*		  anymore. */
/* 061792  Adds two extra polls after ESYNC to catch 'straggler' pods	*/
/* 		  (and shut Jungle Dong up!)*/
/* 061992  Rob fixes stupid mistakes he make for the 061792 version
 */
/* 062492  Rob fixes other stupid mistake in the 061992 version */
/* 072392  Rob just can't seem to get the DC transfer right !   */
/* 083092	Should have fixed the HOP in free fo all problem
                          Filter bad codes from going to the PC.
                          Can cause unsightly side effects.
*/
/*And hopefully didn't create more.*/

#define __TOPLEV

#include "gamma.h"
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "asd.h"
#include "video.h"
#define CRED 4
/*GLOBAL FOR USE ONLY IN POLL !*/
struct com_5 poll_c5;

void(interrupt far *oldtimer)(void);

/*Just guess this one.*/

main(int argc, char **argv) {
  int i, j;
  char ts[80];
  char quit, temp;
  byte track_lut_tmp[21] = {1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4,
                            4, 5, 5, 5, 6, 6, 1, 7, 3, 8};
  byte time_info_tmp[4] = {61, 61, 61, 62};
  /*************************************
  SETUP GLOBAL VARIABLES THAT WERE DEFINED IN PHOTON.H
  *************************************/
  poll_on = 0;
  sync = NSYNC;
  ItsFake = 0;
  poll_val = 0x00;
  slot = 00;
  txval = 0x00;
  red_down = 00;
  poll_flag = FALSE;
  tickcount = 0;
  monscreen = MONMEM;
  track_lut = track_lut_tmp;
  time_info = time_info_tmp;
  time_info_num = 0;
  TCEnable = FALSE;
  strncpy(password, "OriginpfjS", 10);
  for (i = 0; i < 0xff; ++i) slotir[i] = 0;
  for (i = 0; i < 21; ++i)
    for (j = 0; j < 4; ++j) irslot[i][j] = 0;
  /**********************************/

  if (ASDInit(FINDADAPTER | CO40)) {
    printf("Error initializing video system.\n");
    exit(-1);
  }

  MonoCursorOff();

  vPage(0);
  vBlinkControl(BLOFF);
  vCursorControl(CUR_HIDDEN);
  vPage(1);

  srand((int)(inp(0x40))); /*seed random # generator*/

  MonoPalette();
  /*validate("gamma.exe",checksum);*/ /*validate executable file*/

  ControlBootScreen();
  setupcga(BOOT);
  while (!kbhit())
    ;

  loadconfig("system.ini"); /*load default configuration*/
  sys_config(FALSE);        /*configure*/

  if (argc == 1) {
    LoadWeeklyInfo("system.cur");
    update_weekly = 1;
  } else {
    LoadWeeklyInfo(0);
    if (!strcmp("rc", argv[1]))
      update_weekly = 1;
    else
      update_weekly = 0;
  }

  SetupMonoIdle(0); /*setup monitor screen*/

  info("-Init IRSLOT");
  setupslots(); /*initialize irslot & slotir arrays*/

  info("-Init Hostess IO ports");
  setuphostess(); /*initialize hostess ports*/

  info("-Init PODID");
  SetupPodId(); /*initialize sig->id id->sig arrays*/

  info("-Install INT24 handler");
  _harderr(hardhandler); /*kill abort, retry, fail messages*/

  setupcga(BOOT);
  sync = NSYNC; /*set sync to none*/

  buffer = (char *)malloc(2051);
  /*GET CURRENT SYSTEM TIME AND DATE*/
  _dos_gettime(&curtime);
  _dos_getdate(&curdate);

  info("-Installing slave...");

  oldtimer = _dos_getvect(0x1c); /*get old interrupt vector*/
  _dos_setvect(0x1c, poll);      /*replace old with new*/
  outp(0x43, 0x36);              /*speed up timer*/
  outp(0x40, 0x38);
  outp(0x40, 0x4c);
  info("...successsful");
  outp(0x203, 0x80); /*shut up slave*/
  outp(0x200, 0x20);

  if (curconfig.pc == TRUE) {
    info("-Initializing Progress Computer");
    charout(PC, 0xE1);
  }

  info("-Initializing Effects Computer");
  if (curconfig.ec == TRUE) /*initialize effects*/
  {
    effectsout("\rLOAD M\rRU50\r", 13);
  }

  vPage(0);
  vStatLine("ESTABLISHING ET COMMUNICATIONS", 3, COLOR(BLK, WHT), 1);
  vPage(1);

  info("-Initialize Et's");
  ConfigEts(FALSE); /*configure entry terminals*/

  info("Stop/Pause CD PLAYER");
  CD_stop(); /*reset CD player*/
  CD_pause();

  for (i = 0; i < 10; ++i) /*clear textinfo array*/
    for (j = 0; j < 40; ++j) textinfo[i][j] = 0;
  clrcga();            /*clear display*/
  setupcga(TWEENTEXT); /*display 'tween screen*/

  sent = TRUE; /*default sent to be true*/

  MainLoop();

  ResetIBM(); /*Reset timer, close files, etc*/
  return (0);
}
struct gamerec rec;

void LoadWeeklyInfo(char *filename) {
  FILE *indata;
  if ((filename) && (indata = fopen(filename, "rb")))
    fread(&weekly, sizeof(weekly), 1, indata);
  else {
    weekly.game_number = 1;
    weekly.highscore.passport[0] = 0;
    weekly.highhits.passport[0] = 0;
  }
  game.number = weekly.game_number;
  fclose(indata);
}

void UpdateWeeklyInfo(char *filename) {
  FILE *indata;

  if (update_weekly != 1) return;

  weekly.game_number = game.number;

  if (indata = fopen(filename, "wb"))
    fwrite(&weekly, sizeof(weekly), 1, indata);
  else
    info("error history file...");
  fclose(indata);
}

void MonoCursorOff(void) {
  outp(0x3b4, 14);
  outp(0x3b5, 2);
  outp(0x3b4, 15);
  outp(0x3b5, 81);
}

void LoadFakeGame(char *name) {
  FILE *indata;
  char *ts[80];
  int i;
  struct pc_playerinfo tempdata;
  byte *data;
  byte *dest;

  indata = fopen(name, "rb");

  if (indata == NULL) exit(-1);

  fread(&rec, sizeof(struct gamerec), 1, indata);
  i = game.number;
  memcpy(&game, &rec.game, sizeof(struct gamestruct));
  game.number = i;
  for (i = 1; i < 41; ++i) {
    memcpy(player[i].name, rec.player[i - 1].name, 10);
    if (rec.player[i - 1].used) memcpy(player[i].passport, "0000000010", 10);
  }

  /*    memcpy(&RadioData[0], &rec.GameData[0], 360 * 40);*/

  ItsFake = 1;

  fclose(indata);
}

void InitGameData(void) {
  int l;
  /* Clear Out Player Records */
  for (l = 0; l < 40; ++l) /*clear player records from prev. game*/
  {
    player[l].used = FALSE;
    memset(player[l].passport, 32, 10);
    memset(player[l].name, 32, 10);
    player[l].score = 0;
  }

  /* 'reset' the game settings*/
  game.mode1 = PUBLIC;
  game.field = 1;
  player[40].score = 0;
  player[41].score = 0;
  game.length = 360;

  /* Reset the Team Names */
  memset(game.redtm1, 32, 10);
  memset(game.redtm2, 32, 10);
  memset(game.grntm1, 32, 10);
  memset(game.grntm2, 32, 10);

  /*reset player count & down queue*/
  red_down = 0;        /*clear down slots*/
  players[RED][0] = 0; /*clear player records*/
  players[RED][1] = 0;
  players[GREEN][0] = 0;
  players[GREEN][1] = 0;
}

byte handle_pregame(void) {
  int l, n;
  byte tcount;
  byte character;
  byte tokn, bye;
  byte *Et1, *Et2;
  byte Eta, Etb;
  byte recd, recd1;
  struct com_5 status;
  int minute;
  int two_count = 0;
  char ts[80];

  tokn = TRUE;  /*tokn screen has not been displayed yet*/
  bye = FALSE;  /*default, not ctrl_q'd*/
  sync = PSYNC; /*set sync to PSYNC*/
  recd = 0;     /*init. receive character buffers*/
  recd1 = 0;

  ItsFake = 0;

  /* Reset PC */
  if (curconfig.pc == TRUE) charout(PC, 0xE7);

  InitGameData();

  /*Set where the status line is*/

  if (sent == TRUE) /*set status line to */
    stl = 24;       /*appropriate location*/
  else if (game.field == 2)
    stl = 12;
  else if (game.field == 1)
    stl = 24;

AfterReConfig: /*Re-entrance if a ctrl=r is done...*/
  if ((curconfig.et1 == RED) ||
      (curconfig.et2 == GREEN)) /*set Et's to their right places*/
  {
    Et1 = &Eta;
    Et2 = &Etb;
  } else if ((curconfig.et1 == GREEN) || (curconfig.et2 == RED)) {
    Et1 = &Etb;
    Et2 = &Eta;
  } else {
    Et1 = &Eta;
    Et2 = &Etb;
  }
  if (curconfig.et1 == 0) /*ignore not used et's*/
    Eta = 4;
  else
    Eta = 0;
  if (curconfig.et2 == 0)
    Etb = 4;
  else
    Etb = 0;
  if (curconfig.etfake == 1) Eta = 5;
  if (curconfig.etfake == 2) Etb = 5;

  if (Eta != 4) outp(ET1, ETPOLL); /*poll et's*/
  if (Etb != 4) outp(ET2, ETPOLL);
  EtStatus(*Et1, *Et2); /*display et status*/

  if ((Eta >= 4) && (Etb >= 4)) {
    LoadFakeGame("Fake.gme");
  }

  /*while(getch() != 'r');*/
  Eta = 1;

  while (((Eta < 4) || (Etb < 4)) &
         (bye == FALSE)) /*if both haven't started or ctrl_q'd*/
  {
    if (kbhit()) /*check keyboard*/
    {
      character = getch();
      if (character == 'f') {
        /*Fake Game Mode!*/
        Eta = 5;
        Etb = 5;
        LoadFakeGame("Fake.gme");
      }
      if (character == 17) /*ctrl_q option*/
        bye = TRUE;
      else if ((character == 'a') || (character == 'A')) {
        charout(DC, 0xF8);
        info("Printall Toggle");
      } else if ((character == 'l') || (character == 'L')) {
        charout(DC, 0xE9);
        info("Postgame Printout Requested");
      } else if (('1' <= character) & ('8' >= character)) /*select tracks*/
      {
        info("Sending new track to CD/EFFECTS");
        gametrack = character - 48;
        CD_track(gametrack); /*send track to cd*/
        if (curconfig.ec == TRUE) {
          effectsout("\b\b\b", 3); /*back space old ru command*/
          settrack();              /*send ru(x) to effects*/
        }
      } else if (character == 18) /*re-configure option*/
      {
        setupmenu();
        sys_config(TRUE);
        ConfigEts(TRUE);
        goto AfterReConfig;
      }
    }
    /*Check ET#1*/
    if (Eta < 4) /*if et1 hasn't started*/
    {
      ET_CheckDrop(ET1);
      ReadPort(ET1 + 5, &status);
      if (status.DR == 1) /*if there is a character to read*/
      {
        recd = inp(ET1); /*read character*/
        if (recd == REQXFER) {
          Eta = 1;              /*set Eta status to Reading*/
          EtStatus(*Et1, *Et2); /*display et status*/

          UpdateMonoIdle(SENDRED, recd1,
                         gametrack); /*display it on th monitor*/
          ReadXfer(ET1);             /*read the transfer*/
          Eta = 2;                   /*set Eta to Awaiting Et Start*/
          EtStatus(*Et1, *Et2);      /*display et status*/

        } else if (recd == WAITGC) /*if ET START*/
        {
          Eta = 3; /*set to Waiting field start*/
          if ((Etb >= 3) & (tokn == TRUE)) {
            tokn = FALSE; /*set tokn screen to displayed*/
            clrcga();
            setupcga(INSTOKN); /*display token screen*/
            stl = 20;          /*move status line*/
          }
          EtStatus(*Et1, *Et2); /*display et status*/

        } else if (recd == START) {
          Eta = 5;              /*Set Eta to Start rec'd*/
          EtStatus(*Et1, *Et2); /*display et status*/
        }
      }
      outp(ET1, ETPOLL); /*send poll character*/
    }
    if (Etb < 4) /*same as previous routine*/
    {
      ET_CheckDrop(ET2);
      ReadPort(ET2 + 5, &status);
      if (status.DR == 1) {
        recd1 = inp(ET2);
        if (recd1 == REQXFER) {
          Etb = 1;
          EtStatus(*Et1, *Et2);
          UpdateMonoIdle(recd, SENDGRN, gametrack);
          ReadXfer(ET2);
          Etb = 2;
          EtStatus(*Et1, *Et2);

        } else if (recd1 == WAITGC) {
          Etb = 3;
          if ((Eta >= 3) & (tokn == TRUE)) {
            tokn = FALSE;
            clrcga();
            setupcga(INSTOKN);
            stl = 20;
          }
          EtStatus(*Et1, *Et2);

        } else if (recd1 == START) {
          Etb = 5;
          EtStatus(*Et1, *Et2);
        }
      }
      outp(ET2, ETPOLL);
    }
    if (Eta == 4) recd = 0xdd;
    if (Etb == 4) recd1 = 0xdd;
    UpdateMonoIdle(recd, recd1, gametrack);
    /* check to see if the time has change alot...if it has,
       update the time on the screen*/
    if (!noxfer) {
      if (curtime.minute != minute) {
        minute = curtime.minute;
        vPage(0);
        vChangeAttr(COLOR(BLK, WHT));
        vPosCur(4, 22);
        sprintf(ts, "%2d:%02d", (int)curtime.hour, (int)curtime.minute);
        v_sends(ts);
        if (two_count) {
          two_count = 0;
          PrintMessage(++tnum);
        } else {
          two_count = 1;
        }
      }
      vPage(1);
    }
  }
  if (bye == TRUE)
    return (TRUE);
  else
    return (FALSE);
}

byte PC_spcl(byte info) {
  switch (info) {
    case 0xE1:
    case 0xE2:
    case 0xE3:
    case 0xE4:
    case 0xE5:
    case 0xE6:
    case 0xE7:
    case 0xE9:
    case 0xEA:
    case 0xEE:
      return 1;
      break;
    default:
      return 0;
  }
}

void SetupPlayerData(void) {
  int bs, l;

  if (game.mode1 == PUBLIC) bs = curconfig.pub_begin;
  if (game.mode1 == FREEFORALL) bs = curconfig.ffa_begin;
  if (game.mode1 == LEAGUE) bs = curconfig.lea_begin;

  beginscore = bs;
  player[41].used = TRUE; /*initialize test slots*/
  player[42].used = TRUE;
  player[41].score = bs;
  player[42].score = bs;

  for (l = 0; l < 40; ++l) rankplayer[l] = NULL;

  if (game.field == 1) /*fill game for one field*/
  {
    for (l = 0; l < 20; ++l) /*remember*/
    {
#ifdef DEBUG
      player[l + 1].passport[0] = '0';
#endif
      if (player[l + 1].passport[0] !=
          ' ') /*player is used if ch#1 of passport isn't a space*/
      {
        ++players[RED][0]; /*another player*/
        rankplayer[players[RED][0]] =
            &player[(l + 1)];           /*put him in the ranking array*/
        player[l + 1].score = bs;       /*initialize his score*/
        player[l + 1].baseflag = FALSE; /*he hasn't hit base*/
        player[l + 1].used = TRUE;      /*his slot is used*/
        player[l + 1].handihits = 0;    /*initialize handicapp*/
        player[l + 1].hits = 0;
        player[l + 1].gethit = 0;
        player[l + 1].hitown = 0;
#ifdef DEBUG
        player[l + 1].name[l] = '%';
#endif
      }
#ifdef DEBUG
      player[l + 21].passport[0] = '0';
#endif
      if (player[l + 21].passport[0] != ' ') {
        ++players[GREEN][0];
        rankplayer[players[GREEN][0] + 20] = &player[(l + 21)];
        player[l + 21].score = bs;
        player[l + 21].used = TRUE;
        player[l + 21].baseflag = FALSE;
        player[l + 21].handihits = 0;
        player[l + 21].hits = 0;
        player[l + 21].gethit = 0;
        player[l + 21].hitown = 0;
#ifdef DEBUG
        player[l + 21].name[l] = '%';
#endif
      }
    }
  }
  /*NOW THE A/O GAME*/
  else {
    if (game.mode1 == PUBLIC) bs = curconfig.pub_begin;
    if (game.mode1 == FREEFORALL) bs = curconfig.ffa_begin;
    if (game.mode1 == LEAGUE) bs = curconfig.lea_begin;
    for (l = 0; l < 20; l += 2) {
      /*RED guy*/
      if (player[l + 1].passport[0] != ' ') {
        ++players[RED][0];
        rankplayer[(players[RED][0] * 2 - 1)] = &player[(l + 1)];
        player[l + 1].score = bs;
        player[l + 1].baseflag = FALSE;
        player[l + 1].used = TRUE;
        player[l + 1].handihits = 0;
        player[l + 1].hits = 0;
        player[l + 1].gethit = 0;
        player[l + 1].hitown = 0;
      }
      if (player[l + 21].passport[0] != ' ') {
        ++players[GREEN][0];
        rankplayer[(players[GREEN][0] * 2 + 19)] = &player[(l + 21)];
        player[l + 21].score = bs;
        player[l + 21].used = TRUE;
        player[l + 21].baseflag = FALSE;
        player[l + 21].handihits = 0;
        player[l + 21].hits = 0;
        player[l + 21].gethit = 0;
        player[l + 21].hitown = 0;
      }
    }
    if (game.mode2 == PUBLIC) bs = curconfig.pub_begin;
    if (game.mode2 == FREEFORALL) bs = curconfig.ffa_begin;
    if (game.mode2 == LEAGUE) bs = curconfig.lea_begin;
    for (l = 1; l < 20; l += 2) {
      /*RED guy*/
      if (player[l + 1].passport[0] != ' ') {
        ++players[RED][1];
        rankplayer[players[RED][1] * 2] = &player[(l + 1)];
        player[l + 1].score = bs;
        player[l + 1].baseflag = FALSE;
        player[l + 1].used = TRUE;
        player[l + 1].handihits = 0;
        player[l + 1].hits = 0;
        player[l + 1].gethit = 0;
        player[l + 1].hitown = 0;
      }
      if (player[l + 21].passport[0] != ' ') {
        ++players[GREEN][1];
        rankplayer[players[GREEN][1] * 2 + 20] = &player[(l + 21)];
        player[l + 21].score = bs;
        player[l + 21].used = TRUE;
        player[l + 21].baseflag = FALSE;
        player[l + 21].handihits = 0;
        player[l + 21].hits = 0;
        player[l + 21].gethit = 0;
        player[l + 21].hitown = 0;
      }
    }
  }
}

byte game_dokeyboard(void) {
  char gotch;
  static int pl;
  static byte view = 0;
  if (kbhit()) {
    gotch = getch();
    pl = 10;
    /*if (gotch == password[0])
            pl = 1;
    else if (password[pl] == gotch)
            if (pl < 10) ++pl;*/
    switch (gotch) {
      case 'l':
      case 'L':
        charout(DC, 0xE9);
        break;
      case 'a':
      case 'A':
        charout(DC, 0xF8);
        break;
      case 5: /*CTRL-E !*/
        timeremain = 1;
        ctrl_e = TRUE;
        break;
      case 'r': /*RADIO VIEW*/
        if (pl >= 10) view = 2;
        break;
      case 'n':
        view = 0;
        break;
      case 'p':
        view = 1;
        break;
      case 'P':
        view = 6;
        break;
      case 's':
        view = 3;
        break;
      case 'c':
        view = 5;
        break;
    }
  }
  return (view);
}

void CheckForHighScores(void) {
  /*just simply scan all player records for possible high scores*/

  int i = 1;

  while (i < 42) {
    if (player[i].passport[0] != ' ') {
      if ((weekly.highscore.passport[0] == 0) ||
          (player[i].score > weekly.highscore.score))
        memcpy(&weekly.highscore, &player[i], sizeof(struct playertype));
      if ((weekly.highhits.passport[0] == 0) ||
          (player[i].hits > weekly.highhits.hits))
        memcpy(&weekly.highhits, &player[i], sizeof(struct playertype));
    }
    ++i;
  }
}

void rungame(void) {
  int bs, l, n;
  int sbegin, send;
  char tempst[40], ts[80];
  byte view;
  byte cdf = FALSE;
  byte pcs = FALSE;

  /*Tell them we're here!*/
  info("GAME START Received");

  SetupPlayerData();

  /*reset counter*/
  PollNum = 0;

  vPage(0);

  /*clear out poll array*/
  for (l = 0; l < 65; ++l) npoll[l] = 0;

  /*setup location of status line for two field operation*/
  /*...only counts when we're in fuckedup mode, really*/
  if (game.field == 1) stl = 24;
  if (game.field == 2) stl = 12;

  /*notify lobby of impending game start*/
  setupcga(ALRTSCRN);
  if (curconfig.pc) charout(PC, 0xE7);

  /*syncronize watches!*/
  poll_flag = FALSE;
  while (poll_flag == FALSE)
    ;

  /*set the timer and start it counting down.*/
  timeremain = 29;
  TCEnable = TRUE;

  /*send start signal to effects computer*/
  info("Sent start to EFFECTS");
  effectsout("\r", 1);

  /*make SURE the sync is in PSYNC mode*/
  sync = PSYNC;

  /*Tell PC to expect data*/
  if (curconfig.pc == TRUE) charout(PC, 0xE2);

  /*The PRE-GAME countdown has started!*/
  while (timeremain > 0) {
    if (poll_flag == TRUE) {
      poll_flag = FALSE;
      /*print COUNTDOWN on status line*/
      vPage(0);
      vChangeAttr(COLOR(CRED, BLK));
      Dig_Digit((int)(timeremain / 10), 15, 4);
      Dig_Digit((int)(timeremain % 10), 23, 4);

      vPage(1);

      /*send data to the PC*/
      if ((timeremain <= 25) && (pcs == FALSE) && (curconfig.pc == TRUE)) {
        info("Sending data to PC");
        for (l = 0; l < 40; ++l) {
          for (n = 0; n < 10; ++n) charout(PC, player[l + 1].name[n]);
          charout(PC, (byte)player[l + 1].used);
        }
        pcs = TRUE;
      }

      /*START CD player*/
      if ((timeremain <= 19) && (cdf == FALSE)) {
        cdf = TRUE;
        CD_pause();
        info("Sent start to CD");

        /*make sure PC is ready!*/
        if (curconfig.pc == TRUE) charout(PC, 0xE3);
      }
    }
  }

  /*CLEAR ET'S for the next game*/
  if (curconfig.et1 != 0) /*Let the ET's go...*/
    outp(ET1, STARTACKN);
  if (curconfig.et2 != 0) outp(ET2, STARTACKN);

  /*setup monitor screen to game mode*/
  SetupMonoGame();

  /*make pods think the game has started and begin to process data*/
  sync = GSYNC; /*it's GSYNC TIME!*/
  poll_on = 1;
  timeremain = game.length;

  /*Set lobby screen to game mode*/

  if (game.field == 1) {
    setupcga(GAME1); /*single field game*/
    stl = 24;        /*set status line*/
  } else {
    setupcga(GAME2); /*dual field game*/
    stl = 12;        /*set status line*/
    vPage(0);
    vPosCur(10, 12);
    vChangeAttr(COLOR(HWHT, BLU));
    v_printf("%2d", game.number);
    vPosCur(5, 13);
    vChangeAttr(COLOR(HWHT, BLK));
    v_printf("%10s", game.redtm2);
    vPosCur(25, 13);
    v_printf("%10s", game.grntm2);
    vPage(1);
  }

  UpdateSync(7, 21, sync);
  UpdateView(0);

  /*this here is THE game...*/

  for (; timeremain > -3;) {
    if (poll_flag == TRUE) {
      poll_flag = FALSE;
      sbegin = slot;
      PollNum++;

      if (timeremain <= 0) {
        sync = ESYNC;
        UpdateSync(7, 21, sync);
        vPage(0);
        vStatLine("<< TERMINATED >>", 24, COLOR(HWHT, HBLU), 1);
        vPage(1);
      }

      /* download data to the PC*/
      if (curconfig.pc == TRUE) {
        outp(PC, 0xE4);
        for (l = 5; l < 25; ++l) {
          if ((npoll[l] != 0x00) && (npoll[l] < 0xE0))
            charout(PC, npoll[l]);
          else if ((pod[l - 4].resetflag == '*') && (pod[l - 4].podid != 0))
            charout(PC, 0xEE);
          else
            charout(PC, 0x10);
        }
        for (l = 31; l < 51; ++l) {
          if ((npoll[l] != 0x00) && (npoll[l] < 0xE0))
            charout(PC, npoll[l]);
          else if ((pod[l - 10].resetflag == '*') && (pod[l - 10].podid != 0))
            charout(PC, 0xEE);
          else
            charout(PC, 0x10);
        }
        /*send current time and manuever number*/
        charout(PC, (byte)(game.number >> 8));
        charout(PC, (byte)(game.number & 0xff));
        charout(PC, (byte)(timeremain >> 8));
        charout(PC, (byte)(timeremain & 0xff));
        charout(PC, 0xE5);
      }

      view = game_dokeyboard();

      /*mark reset pods*/
      for (l = 1; l < 41; ++l)
        if (pod[l].missinrow > 30) pod[l].resetflag = '*';

      /*print the time remaining on both screens*/
      sprintf(tempst, "%2d:%02d", (int)(timeremain / 60),
              (int)(timeremain % 60));
      if (timeremain > 0) {
        vPage(0);
        vStatLine(tempst, 34, COLOR(HWHT, BLU), 0);
        vPage(1);
      }
      PrintMono(6, 21, tempst);

      /*update the scores on the on the lobby screen*/
      doscores();

      /*update the test slot score on the tech-screen*/
      sprintf(tempst, "%5d", player[41].score);
      PrintMono(5, 41, tempst);
      sprintf(tempst, "%5d", player[42].score);
      PrintMono(6, 41, tempst);
      send = slot;
      sprintf(tempst, "%2d-%02d-%02d      %2d:%02d:%02d", curdate.month,
              curdate.day, curdate.year, curtime.hour, curtime.minute,
              curtime.second);
      PrintMono(3, 25, tempst);
      vPage(0);
      vChangeAttr(COLOR(HWHT, BLU));
      vPosCur(18, 23);
      /*v_printf("   %2d:%02d - %2d-%02d-%04d ", curtime.hour, curtime.minute,
       * curdate.month, curdate.day, curdate.year);*/
      vPage(1);
      sprintf(tempst, "BS:%3d ES:%3d RB:%d", sbegin, send, badradio);
      PrintMono(10, 1, tempst);
      UpdateView(view);
    }
  }

  /*end of the game...clean up*/
  view = 0;
  sync = ESYNC;
  poll_on = 0;

  if ((curconfig.pc == TRUE) && (curconfig.pcmode >= 1))
    if (((game.mode1 == LEAGUE) || (game.mode2 == LEAGUE)) ||
        (curconfig.pcmode == 2)) {
      info("ENQ");
      charout(PC, 0xE6); /*Enquire tournament mode?*/
    }

  UpdateSync(7, 21, sync);

  vPage(0);
  vStatLine("<< TERMINATED >>", 24, COLOR(HWHT, HBLU), 1);
  vPage(1);

  /*final score update*/
  doscores();

  /*ESYNC time*/
  timeremain = 17;

  /*download score data to PC (if it want's it)*/
  if ((curconfig.pc == TRUE) && (curconfig.pcmode >= 1))
    if (((game.mode1 == LEAGUE) || (game.mode2 == LEAGUE)) ||
        (curconfig.pcmode == 2)) {
      HOST_sendsn(PC, &curtime, sizeof(curtime));
      HOST_sendsn(PC, &curdate, sizeof(curdate));
      for (l = 1; l < 41; ++l) {
        HOST_sendsn(PC, &player[l].score, sizeof(int));
        HOST_sendsn(PC, &pod[l], sizeof(struct podstruct));
      }
      HOST_sendsn(PC, &game, sizeof(struct gamestruct));
      info("Data Done!");
      charout(PC, 0xE9);
    }

  for (; timeremain > 0;) {
  }
  /*stop the CD*/

  CD_pause();

  /*make the PC go to sleep*/
  if (curconfig.pc == TRUE) charout(PC, 0xE7);

  /*stop the time from counting down*/
  TCEnable = FALSE;
  sync = NSYNC; /*stop sync*/

  ++game.number;
  CheckForHighScores(); /*increment game #*/
  vPage(1);
}

/**************************************************************************
/     The poll routine, the heart of the program...
/     Just put it this - a - way, it's not commented so you don't know
/     WHY or HOW it works... (this is for wes if he ever gets my source)
***************************************************************************/
/* a 'poll_c5' struct is used here that's global.*/
#pragma optimize("", off)
void interrupt far poll() {
  static byte tickcount = 0;
  _disable();
  _asm mov WORD PTR appstack[0], sp;
  _asm mov WORD PTR appstack[2], ss;
  _enable();
  if (slot >= 67) {
    if (sync == PSYNC) {
      keyup();
      outp(RADIO, PSYNC);
    }
    if (sync == ESYNC) {
      keyup();
      outp(RADIO, ESYNC);
    }
    if (sync == GSYNC) {
      outp(RADIO, GSYNC);
      outp(PS, GSYNC);
    }
    slot = 0;
    mono(0, 7, 23) = '*';
  } else if (poll_on == 1) {
    if ((slot > 2) && (slot < 25)) {
      keydown();
      if ((ItsFake) && (slot > 4)) {
        if ((rec.GameData[PollNum][slot - 5] != 0x00) &&
            (rec.GameData[PollNum][slot - 5] != 0xee)) {
          poll_val = rec.GameData[PollNum][slot - 5];
          npoll[slot] = poll_val;
          if (player[slot - 4].used) processpoll(slot - 4, poll_val, RED);
        } else {
          npoll[slot] = 0;
        }
      } else {
        ReadPort(RADIO + 5, &poll_c5);
        if (poll_c5.DR == 1) {
          poll_val = inp(RADIO);
          if (slot > 4) {
            if (poll_val == 0x39) ++badradio;
            npoll[slot] = poll_val;
            if (player[slot - 4].used) processpoll(slot - 4, poll_val, RED);
          } else if (slot <= 4) {
            processpoll(slot + 38, poll_val, TEST);
          }
        } else {
          npoll[slot] = 00;
          if (player[slot - 4].used == TRUE) {
            ++pod[slot - 4].txmiss;
            ++pod[slot - 4].missinrow;
          }
        }
      }
    }
    if ((slot > 24) && (slot < 30)) {
      if (red_down > 0) {
        txval = putdown(RED);
        outp(RADIO, txval);
        npoll[slot] = txval;
      } else
        npoll[slot] = 0;
    } else if (slot == 30)
      npoll[slot] = inp(RADIO);
    else if ((slot > 30) && (slot < 51)) {
      if (ItsFake) {
        if ((rec.GameData[PollNum][slot - 11] != 0x00) &&
            (rec.GameData[PollNum][slot - 5] != 0xee)) {
          poll_val = rec.GameData[PollNum][slot - 11];
          npoll[slot] = poll_val;
          if (player[slot - 10].used) processpoll(slot - 10, poll_val, GREEN);
        } else {
          npoll[slot] = 0;
        }
      } else {
        if (inp(RADIO + 2) == 4) {
          poll_val = inp(RADIO);
          npoll[slot] = poll_val;
          if (player[slot - 10].used) processpoll(slot - 10, poll_val, GREEN);
        } else {
          npoll[slot] = 00;
          if (player[slot - 10].used == TRUE) {
            ++pod[slot - 10].txmiss;
            ++pod[slot - 10].missinrow;
          }
        }
      }
    } else if ((slot > 54) && (slot < 60)) {
      if (red_down > 0) {
        txval = putdown(RED);
        outp(RADIO, txval);
        npoll[slot] = txval;
      } else
        npoll[slot] = 0;
    }
  }
  if (slot == 56) poll_flag = TRUE;
  if (poll_on == 1) {
    if (slot == 65)
      keyup();
    else if (slot == 1)
      keydown();
    else if (slot == 24)
      keyup();
    else if (slot == 30)
      keydown();
    else if (slot == 53)
      keyup();
  }
  if (slot == 3) mono(0, 7, 23) = ' ';
  ++slot;
  ++tickcount;
  if (tickcount >= time_info[time_info_num]) {
    seconds = 1;
    IncTime();
    tickcount = 0;
    time_info_num = (time_info_num + 1) % 4;
    if (TCEnable == TRUE) --timeremain;
  }
  _disable();
  _asm mov sp, WORD PTR appstack[0];
  _asm mov ss, WORD PTR appstack[2];

  _enable();
  outp(0x20, 0x20);
}
/*********************OK, YOU CAN KNOW WHAT THE REST DOES>>>*************/
#pragma optimize("", on)

void loadconfig(char *configfile) /*Load a config structure from disk*/
{
  FILE *fp;
  char answer;
  if ((fp = fopen(configfile, "r")) == NULL) {
    printf("Initialization file not available...\n");
    printf("Please specify your preferences:\n");
    setupmenu();
  } else {
    fread(&newconfig, sizeof(newconfig), 1, fp);
    fclose(fp);
  }
}

void processpoll_f(void) /*Function for processpoll*/
{
  pod[slotnum].missinrow = 0; /*It's received SOMETHING!*/
  if (slotnum % 2 == 0)
    gameslot = game.mode1; /* should be game.mode2, but this is only a test...*/
  else
    gameslot = game.mode1;
  if (player[slotnum].used == TRUE) /*Don't process it unless its used*/
  {                                 /* GET BASE */
    if (value != 0x80) {
      if ((value == 0x2B) && (gameslot != FREEFORALL)) /*Red Base Check*/
      {
        ++pod[slotnum].base;
        if ((color == RED) & (player[slotnum].baseflag == 0)) {
          player[slotnum].baseflag = 1;
          player[slotnum].score = player[slotnum].score + 200;
        }
      } else if ((value == 0x35) &&
                 (gameslot != FREEFORALL)) /*Check green base*/
      {
        ++pod[slotnum].base;
        if ((color == GREEN) & (player[slotnum].baseflag == 0)) {
          player[slotnum].baseflag = 1;
          player[slotnum].score = player[slotnum].score + 200;
        }
      }
      /* HIT OWN PLAYER */
      else if ((color == RED) && (irslot[slotnum][RED] == value) &&
               (game.mode1 != FREEFORALL)) /*Red team/ hit own player*/
      {
        player[slotnum].score = player[slotnum].score - 30;
        ++red_down;
        ++pod[slotnum].hitsent;
        down[red_down] = irslot[slotnum][RED];
        ++pod[slotnum].valcode;
        ++player[slotnum].hitown;
      } else if ((color == GREEN) && (irslot[slotnum - 20][GREEN] == value) &&
                 (game.mode1 != FREEFORALL)) /*Grn team/ hit own player*/
      {
        player[slotnum].score = player[slotnum].score - 30;
        ++red_down;
        ++pod[slotnum].hitsent;
        down[red_down] = irslot[slotnum - 20][GREEN];
        ++pod[slotnum].valcode;
        ++player[slotnum].hitown;
      } else if ((color == TEST) & (irslot[slotnum - 40][TEST] == value) &&
                 (gameslot != FREEFORALL)) /*Test / hit own player*/
      {
        player[slotnum].score = player[slotnum].score - 30;
        ++red_down;
        ++pod[slotnum].hitsent;
        down[red_down] = irslot[slotnum - 40][TEST];
        ++player[slotnum].hitown;
      }
      /* HIT OPPONENT */
      else if (player[slotir[value]].used ==
               TRUE) /*If the code received is used...*/
      {
        vPage(0);
        if (0 < slotir[value] < 41) /*If its a valid codes...*/
        {
          if ((!(((((0 < slotir[value]) &&
                    (slotir[value] <= 20)) && /* IF (NOT (ON SAME TEAM AND NOT
                                                 FFA)) AND NOT HOP */
                   ((0 < slotnum) && (slotnum <= 20))) ||
                  (((20 < slotir[value]) && (slotir[value] <= 40)) &&
                   ((20 < slotnum) && (slotnum <= 40)))) &&
                 (gameslot != FREEFORALL))) &&
              (slotir[value] != slotnum)) {
            ++red_down;
            down[red_down] = value;
            if (slotir[value] <= 40) {
              ++pod[slotir[value]].hitsent;
              ++player[slotir[value]].gethit;
              player[slotnum].score += 10;
              ++pod[slotnum].valcode;
              ++player[slotnum].hits;
              if (slotir[value] % 2 ==
                  0) /*ALPHA/OMEGA different game mode checking.*/
                hcp = game.hcp_b;
              else
                hcp = game.hcp_a;
              if ((hcp == FALSE) ||
                  (player[slotir[value]].score - beginscore > -150))
                player[slotir[value]].score -= 10;
              if ((hcp == TRUE) &&
                  (player[slotir[value]].score - beginscore <= -150)) {
                if (player[slotir[value]].handihits >= 3) {
                  player[slotir[value]].handihits = 0;
                  player[slotir[value]].score -= 10;
                } else
                  ++player[slotir[value]].handihits;
              }
            } else {
              player[slotir[value]].score -= 10;
              ++pod[slotnum].valcode;
            }
          } else {
            npoll[slot] = 0;
            ++pod[slotnum].txbad;
          }
        } else if (40 < slotir[value] < 43) /*Test Pods*/
        {
          ++red_down;
          down[red_down] = value;
          ++pod[slotir[value]].hitsent;
          ++pod[slotnum].valcode;
        }

      } else if (IsId(value) == TRUE) {
        if ((pod[slotnum].podid == 0) && (sigid[value] != 0)) {
          pod[slotnum].podid = sigid[value];
          ++pod[slotnum].id;
        } else if (pod[slotnum].podid == sigid[value])
          ++pod[slotnum].id;
        else
          ++pod[slotnum].txbad;
      } else if ((0xa0 < value) && (value < 0xa7)) {
        pod[slotnum].hitackn += (value - 0xa0);
        ++pod[slotnum].valcode;
      } else if ((0xb0 <= value) && (value <= 0xbf)) {
        ++pod[slotnum].valcode;
        ++pod[slotnum].rxbad;
      } else {
        ++pod[slotnum].txbad;
        npoll[slot] = 0;
      }
    }
  }
}

byte putdown(byte color) {
  pd_j = down[1];
  if (red_down >= 30) red_down = 29;
  for (pd_k = red_down; pd_k > 1; --pd_k) down[pd_k - 1] = (down[pd_k]);
  --red_down;
  return (pd_j);
}

void SortScores() {
  volatile struct playertype *temp;
  int k, j, i;

  if (game.field == 1) {
    for (i = 1; i < players[RED][0]; ++i) {
      k = i;
      for (j = i + 1; j <= players[RED][0]; ++j)
        if (rankplayer[j]->score > rankplayer[k]->score) k = j;
      temp = rankplayer[i];
      rankplayer[i] = rankplayer[k];
      rankplayer[k] = temp;
    }
    for (i = 21; i < players[GREEN][0] + 20; ++i) {
      k = i;
      for (j = i + 1; j <= players[GREEN][0] + 20; ++j)
        if (rankplayer[j]->score > rankplayer[k]->score) k = j;
      temp = rankplayer[i];
      rankplayer[i] = rankplayer[k];
      rankplayer[k] = temp;
    }
  }
  if (game.field == 2) /*For TWO Fields*/
  {
    for (i = 1; i < 2 * players[RED][0]; i += 2) {
      k = i;
      for (j = i + 2; j <= 2 * players[RED][0]; j += 2)
        if (rankplayer[j]->score > rankplayer[k]->score) k = j;
      temp = rankplayer[i];
      rankplayer[i] = rankplayer[k];
      rankplayer[k] = temp;
    }
    for (i = 21; i < 2 * players[GREEN][0] + 19; i += 2) {
      k = i;
      for (j = i + 2; j <= 2 * players[GREEN][0] + 19; j += 2)
        if (rankplayer[j]->score > rankplayer[k]->score) k = j;
      temp = rankplayer[i];
      rankplayer[i] = rankplayer[k];
      rankplayer[k] = temp;
    }
    for (i = 2; i < 2 * players[RED][1]; i += 2) {
      k = i;
      for (j = i + 2; j <= 2 * players[RED][1]; j += 2)
        if (rankplayer[j]->score > rankplayer[k]->score) k = j;
      temp = rankplayer[i];
      rankplayer[i] = rankplayer[k];
      rankplayer[k] = temp;
    }
    for (i = 22; i < 2 * players[GREEN][1] + 20; i += 2) {
      k = i;
      for (j = i + 2; j <= 2 * players[GREEN][1] + 20; j += 2)
        if (rankplayer[j]->score > rankplayer[k]->score) k = j;
      temp = rankplayer[i];
      rankplayer[i] = rankplayer[k];
      rankplayer[k] = temp;
    }
  }
}

void doscores(void) {
  SortScores();

  if (game.field == 1) {
    if ((curconfig.newscr > 1) ||
        ((curconfig.newscr == 1) && (game.mode1 != PUBLIC)))
      LOBBY_game1_update(1);
    else
      LOBBY_game1_update(0);
  }
}

void SendToDC(void) {
  int m, j, k, l, EFLAG, oslot, tick;
  struct tm dcts;
  char ts[40];
  MEM omar;
  l = 0;
  info("-SENT ENQ TO DATA COMPUTER");
  charout(DC, 0xE1);   /*DC Enquiry*/
  WaitAck(DC, 4);      /*Wait 4 cycles for the response*/
  if (inp(DC) == 0xE2) /*If it sent back the correct response..*/
  {
    EFLAG = FALSE;
    info("   ENQ ANSWERED");
    WaitPoll();
    info("-SENDING DATA TO DC");
    sprintf(ts, "   %2d:%02d %2d-%02d-%02d", curtime.hour, curtime.minute,
            curdate.month, curdate.day, curdate.year % 100);
    vPage(0);
    vStatLine(ts, 23, COLOR(HWHT, BLU), 1);
    omar = vPageMem(0);
    for (m = 0; m < 2000; ++m) buffer[m] = *(omar++);

    vPage(0);
    vStatLine(" TRANSMITTING RESULTS TO PLANET EARTH  ", 0, COLOR(HWHT, BLU),
              1);
    vPage(1);
    for (m = 0; m < 2000;
         ++m) /*Send it...wait for a response after each character!*/
    {
      charout(DC, buffer[m]);
      if (WaitAck(DC, 4) == -1) {
        info("!DC TIMEOUT ON CHARTX");
        EFLAG = TRUE;
        goto byeloop;
      }
    }
  byeloop:
    if (EFLAG == FALSE) {
      for (m = 0; ((m < 4) && (inp(DC) != 0xE3)); ++m) WaitPoll();
      if (inp(DC) == 0xE3)
        sent = TRUE;
      else {
        info("! DC TIMEOUT ON END");
        sent = FALSE;
      }
    }
  } else {
    sent = FALSE;
    info("! DC TIMEOUT ON ENQ");
  }
}

#pragma optimize("", off)
void WaitAPoll(void) {
  while (slot % 10 == 0)
    ;
  while (slot % 10 != 0)
    ;
}
#pragma optimize("", on)

void TranslateXfer(unsigned int et) {
  int thiset, thatet;
  int m, o;
  if (et == ET1) {
    thiset = curconfig.et1;
    thatet = curconfig.et2;
  }
  if (et == ET2) {
    thiset = curconfig.et2;
    thatet = curconfig.et1;
  }
  if (((thiset == BOTH) | (thatet == 0)) & (curconfig.field == 1)) {
    /*single field, beta, 1 et*/
    SetGameMode(&game.mode1, transfer[0][1]);
    game.field = 1;
    if (game.mode1 != LEAGUE) {
      if (game.mode1 == FREEFORALL)
        game.hcp_a = curconfig.ffa_hcp;
      else if (game.mode1 == PUBLIC)
        game.hcp_a = curconfig.pub_hcp;
      game.length = curconfig.length;
      for (m = 0; m < 10; ++m) {
        game.redtm1[m] = curconfig.beta_red[m];
        game.grntm1[m] = curconfig.beta_grn[m];
      }
      GetRedTeam(FALSE, BOTH);
      GetGrnTeam(FALSE, BOTH);
    } else {
      game.hcp_a = curconfig.lea_hcp;
      game.length = curconfig.length;
      GetRedTeam(TRUE, BOTH);
      GetGrnTeam(TRUE, BOTH);
    }
  }
  /*single field, beta, 1 et*/
  else if ((thiset == RED) & (curconfig.field == 1)) {
    SetGameMode(&game.mode1, transfer[0][1]);
    game.field = 1;
    if (game.mode1 != LEAGUE) {
      if (game.mode1 == FREEFORALL)
        game.hcp_a = curconfig.ffa_hcp;
      else if (game.mode1 == PUBLIC)
        game.hcp_a = curconfig.pub_hcp;
      game.length = curconfig.length;
      for (m = 0; m < 10; ++m) {
        game.redtm1[m] = curconfig.beta_red[m];
        game.grntm1[m] = curconfig.beta_grn[m];
      }
      GetRedTeam(FALSE, BOTH);
    } else {
      game.hcp_a = curconfig.lea_hcp;
      game.length = curconfig.length;
      GetRedTeam(TRUE, BOTH);
    }
  }
  /*single field, beta, 1 et (red)*/
  else if ((thiset == GREEN) & (curconfig.field == 1)) {
    SetGameMode(&game.mode1, transfer[0][1]);
    game.field = 1;
    if (game.mode1 != LEAGUE) {
      if (game.mode1 == FREEFORALL)
        game.hcp_a = curconfig.ffa_hcp;
      else if (game.mode1 == PUBLIC)
        game.hcp_a = curconfig.pub_hcp;
      game.length = curconfig.length;
      for (m = 0; m < 10; ++m) {
        game.redtm1[m] = curconfig.beta_red[m];
        game.grntm1[m] = curconfig.beta_grn[m];
      }
      GetGrnTeam(FALSE, GREEN);
    } else {
      game.hcp_a = curconfig.lea_hcp;
      game.length = curconfig.length;
      GetGrnTeam(TRUE, GREEN);
    }
  } else if ((thiset == RED) & (curconfig.field == 2)) {
    SetGameMode(&game.mode1, transfer[0][1]);
    if (thatet == OFF)
      game.field = 1;
    else
      game.field = 2;
    if (game.mode1 != LEAGUE) {
      if (game.mode1 == FREEFORALL)
        game.hcp_a = curconfig.ffa_hcp;
      else if (game.mode1 == PUBLIC)
        game.hcp_a = curconfig.pub_hcp;
      game.length = curconfig.length;
      for (m = 0; m < 10; ++m) {
        game.redtm1[m] = curconfig.alpha_red[m];
        game.grntm1[m] = curconfig.alpha_grn[m];
      }
      GetRedTeam(FALSE, ALPHA);
      GetGrnTeam(FALSE, ALPHA);
    } else {
      game.hcp_a = curconfig.lea_hcp;
      game.length = curconfig.length;
      GetRedTeam(TRUE, ALPHA);
      GetGrnTeam(TRUE, ALPHA);
    }
  }
  /*dual field, alpha*/
  else if ((thiset == GREEN) & (curconfig.field == 2)) {
    SetGameMode(&game.mode2, transfer[0][1]);
    if (thatet == OFF)
      game.field = 1;
    else
      game.field = 2;
    if (game.mode2 != LEAGUE) {
      if (game.mode2 == FREEFORALL)
        game.hcp_b = curconfig.ffa_hcp;
      else if (game.mode2 == PUBLIC)
        game.hcp_b = curconfig.pub_hcp;
      game.length = curconfig.length;
      for (m = 0; m < 10; ++m) {
        game.redtm2[m] = curconfig.omega_red[m];
        game.grntm2[m] = curconfig.omega_grn[m];
      }
      GetRedTeam(FALSE, OMEGA);
      GetGrnTeam(FALSE, OMEGA);
    } else {
      game.hcp_b = curconfig.lea_hcp;
      game.length = curconfig.length;
      GetRedTeam(TRUE, OMEGA);
      GetGrnTeam(TRUE, OMEGA);
    }
    if (thatet == OFF) {
      game.mode1 = game.mode2;
      for (m = 0; m < 10; ++m) {
        game.redtm1[m] = game.redtm2[m];
        game.grntm1[m] = game.grntm2[m];
      }
    }
  }
  /*dual field, omega*/
  if (curconfig.field == 1) game.hcp_b = game.hcp_a;
}

void ResetIBM(void) {
  _disable();
  outp(0x43, 0x36); /*RETURN TIMER TO NORMAL*/
  outp(0x40, 0xff);
  outp(0x40, 0xff);
  _dos_setvect(0x1c, oldtimer); /*ReInstall old interrupt vector*/
  _enable();
  _dos_settime(&curtime);
  _dos_setdate(&curdate);
  /*fclose(log_file);*/
  mPopPalette();
  ASDDeInit();
}

void WritePodPerform(void) {
  FILE *fp;
  char path[30];
  int i;
  struct podstruct rechead;
  rechead.podid = 99;
  rechead.id = game.number;
  sprintf(path, "%spod.red", curconfig.pod_path);
  if ((fp = fopen(path, "r")) == NULL)
    info("! ORIGINAL POD FILE NOT FOUND");
  else {
    fclose(fp);
    if ((fp = fopen(path, "ab")) != NULL) {
      info("Writing Red Performance");
      fwrite(&rechead, sizeof(rechead), 1, fp);
      for (i = 1; i < 21; ++i) {
        if ((player[i].used == TRUE) && (pod[i].id > 20))
          fwrite(&pod[i], sizeof(struct podstruct), 1, fp);
      }
      fclose(fp);
    } else {
      info("! Error writing red perf.");
    }
  }
  sprintf(path, "%spod.grn", curconfig.pod_path);
  if ((fp = fopen(path, "r")) == NULL)
    info("! ERROR FINDING FILE");
  else {
    fclose(fp);
    if ((fp = fopen(path, "ab")) != NULL) {
      info("Writing Grn Performance");
      fwrite(&rechead, sizeof(rechead), 1, fp);
      for (i = 21; i < 41; ++i) {
        if ((player[i].used == TRUE) && (pod[i].id > 20))
          fwrite(&pod[i], sizeof(struct podstruct), 1, fp);
      }
      fclose(fp);
    } else {
      info("! Error writing grn perf.");
    }
  }
}

void IncTime(void) {
  minutes = (int)((seconds + curtime.second) / 60);
  hours = (int)((curtime.minute + minutes) / 60);
  curtime.second = (curtime.second + seconds) % 60;
  curtime.minute = (curtime.minute + minutes) % 60;
  curtime.hour = (curtime.hour + hours) % 60;
  if (curtime.hour >= 24) {
    curtime.hour = 0;
    IncDate();
  }
}

void IncDate(void) {
  static byte dm[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  ++curdate.day;
  if ((curdate.day == 29) && (curdate.month == 2) && (curdate.year % 4 == 0))
    /*LEAP YEAR..DO NOTHING*/;
  else if (dm[curdate.month] < curdate.day) {
    ++curdate.month;
    curdate.day = 0;
    if ((curdate.month) >= 13) {
      ++curdate.year;
      curdate.month = 1;
    }
  }
}

void WriteGmData(void) {
  FILE *gd;
  struct time c_time;
  struct date c_date;
  int i;
  char ts[40];

  c_time.minute = curtime.minute;
  c_time.hour = curtime.hour;
  c_time.second = curtime.second;
  c_time.hsecond = curtime.hsecond;

  c_date.year = curdate.year;
  c_date.day = curdate.day;
  c_date.month = curdate.month;

  sprintf(ts, "%sGDATA.DAT", curconfig.data_path);

  if ((gd = fopen(ts, "ab")) != NULL) {
    info("WRITING GDATA.DAT");
    fprintf(gd, "***");
    fwrite(&c_time, sizeof(struct time), 1, gd);
    fwrite(&c_date, sizeof(struct date), 1, gd);
    fwrite(&game.number, sizeof(byte), 1, gd);
    fwrite(&game.field, sizeof(byte), 1, gd);
    fwrite(&curconfig.field, sizeof(byte), 1, gd);
    fwrite(&game.mode1, sizeof(byte), 1, gd);
    fwrite(&game.mode2, sizeof(byte), 1, gd);
    fwrite(game.redtm1, sizeof(char), 10, gd);
    fwrite(game.grntm1, sizeof(char), 10, gd);
    fwrite(game.redtm2, sizeof(char), 10, gd);
    fwrite(game.grntm2, sizeof(char), 10, gd);
    for (i = 1; i < 41; ++i) {
      fwrite((void *)&player[i].passport, sizeof(char), 10, gd);
      fwrite((void *)&player[i].name, sizeof(char), 10, gd);
      fwrite((void *)&player[i].score, sizeof(int), 1, gd);
    }
    fclose(gd);
  } else
    info("! ERROR WRITING GMDATA");
}

#pragma optimize("", off)
byte WaitAck(int prt, int count) {
  int x;
  int quit = FALSE;
  struct com_5 cominfo;
  byte slut;
  x = 0;
  while ((count >= x) && (quit != TRUE)) {
    slut = slot;
    do {
      ReadPort(prt + 5, &cominfo);
      if (cominfo.DR == 1) quit = TRUE;
    } while ((slot == slut) && (quit != TRUE));
    do {
      ReadPort(prt + 5, &cominfo);
      if (cominfo.DR == 1) quit = TRUE;
    } while ((slot != slut) && (quit != TRUE));
    if (quit == TRUE) inp(prt);
    ++x;
  }
  if (quit != TRUE)
    return (-1);
  else
    return (0);
}
#pragma optimize("", on)

HOST_sendsn(unsigned port, char *string, int size) {
  while (size--) charout(PC, *string++);
}

void ControlBootScreen() {
  char *ts;

  ts = (char *)malloc(100);

  vPage(1);
  vPosCur(0, 0);
  vChangeAttr(hrcNORM);
  vBox(0xffff, '°', 80, 25);
  vChangeAttr(hrcINTEN | hrcREV);
  vBox(0xffff, ' ', 80, 5);
  vBorder(80, 6, 2);
  vPosCur(2, 2);
  sprintf(ts, "PHOTON Gamma System, Revision %s  Date:%s", SYSTEMVERSION,
          SYSTEMDATE);
  v_sends(ts);
  vPosCur(2, 3);
  sprintf(ts, "(c) 1991,1992 Robert Banz, All Rights Reserved");
  v_sends(ts);
  vPosCur(2, 4);
  sprintf(ts,
          "Video routines (And even more shit ) (c) 1991,1992 Chris Fanning");
  vPosCur(0, 24);
  vChangeAttr(hrcNORM);
  v_sends("Press any key to continue...");

  free(ts);
}

void MainLoop() {
  char quit, temp;
  int i, j;

  do /*game loop*/
  {
    SetupMonoIdle(1); /*resetup monitor screen*/
    info("...Updating setup file");
    if (update_weekly) UpdateWeeklyInfo("system.cur");
    info("<<<< NEW GAME >>>>");
    info(" ");
    info("Sending track to CD & Effects");
    SelectTrack(0); /*pick game track*/
    info("-SENT PRINT TO DC");
    charout(DC, 0xE7);
    charout(DC, 0xE4); /*SEND TO DC GO-AND-PRINT*/
#ifdef DEBUG
    info(" Awaiting Acknowledge");
#endif
    WaitAck(DC, 4);
#ifdef DEBUG
    info(" ...Acknlowledge Rec'd");
#endif
    for (i = 1; i <= 42; ++i) /*de-allocate slots*/
      player[i].passport[0] = 32;
    for (i = 1; i <= 40; ++i) /*initialize pod performance*/
    {
      pod[i].id = 0;
      pod[i].base = 0;
      pod[i].rxmiss = 0;
      pod[i].rxbad = 0;
      pod[i].txmiss = 0;
      pod[i].txbad = 0;
      pod[i].podid = 0;
      pod[i].hitsent = 0;
      pod[i].hitackn = 0;
      pod[i].valcode = 0;
      pod[i].missinrow = 0;
      pod[i].resetflag = ' ';
      badradio = 0;
    }
    quit = handle_pregame(); /*got to Et/keyboard handler*/
    if (quit != TRUE) {
      ctrl_e = FALSE;
      rungame();        /*run the game*/
      SetupMonoIdle(0); /*setup monitor for idle*/
      vPage(0);
      vStatLine("        SUMMARY", 23, COLOR(HWHT, BLU), 1);
      vPage(1);
      if ((badradio < 30) && (ctrl_e == FALSE))
        WritePodPerform(); /*write pod performance*/
      if ((ctrl_e == FALSE) && (curconfig.savedata))
        WriteGmData(); /*write game data*/
      if (curconfig.dc == ON)
        SendToDC(); /*transfer*/
      else
        sent = FALSE;
      if (sent == TRUE) /*if transfer successful ...*/
      {
        clrcga();
        setupcga(TWEENTEXT);
        noxfer = 0;
      } else {
        setupcga(NOXFER);
        noxfer = 1;
      }
    }
  } while (quit == FALSE);
}

