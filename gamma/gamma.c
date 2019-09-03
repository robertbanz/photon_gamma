/********************/
/*  GAMMA.C 		  */
/********************/

/*OverLord's Phone #*/
/* 360-3745*/
/* Atlas's #
        1-800-FUK-UR-PROGRAM */

/* 060692  Fixed problems with people getting base points in freeforall */
/* 		  (hey, it only happened in Blowcago with their sucky radio!
 */
/* 		  Fixed problems with GC->PGD xfer locking up.	shouldn't
 * happen */
/* 		  anymore.
 */
/* 061792  Adds two extra polls after ESYNC to catch 'straggler' pods	*/
/* 		  (and shut Jungle Dong up!)*/
/* 061992  Rob fixes stupid mistakes he make for the 061792 version
 */
/* 062492  Rob fixes other stupid mistake in the 061992 version */
/* 072392  Rob just can't seem to get the DC transfer right !   */
/* 080192  Rob fixes everything else in his program that is
                                                                now broke
   because of the "Great Atlas's" ideas*/
/* 083092	Should have fixed the HOP in free fo all problem
                          Filter bad codes from going to the PC.	Can cause
   unsightly side effects.
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
main() {
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
  ASDInit(FINDADAPTER | CO40);
  vPage(0);
  vBlinkControl(BLOFF);
  vCursorControl(CUR_HIDDEN);
  vPage(1);
  srand((int)(inp(0x40))); /*seed random # generator*/
  MonoPalette();
  /*validate("gamma.exe",checksum);*/ /*validate executable file*/
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
  while (!kbhit())
    ;
  loadconfig("system.ini");              /*load default configuration*/
  sys_config(FALSE);                     /*configure*/
  sprintf(ts, "%s", curconfig.log_path); /*open log file*/
  /*log_file = fopen(ts,"ab");*/
  SetupMonoIdle(0); /*setup monitor screen*/
  info("-Init IRSLOT");
  setupslots(); /*initialize irslot & slotir arrays*/
  info("-Init Hostess IO ports");
  setuphostess(); /*initialize hostess ports*/
  info("-Init PODID");
  SetupPodId(); /*initialize sig->id id->sig arrays*/
  info("-Install INT24 handler");
  _harderr(hardhandler);   /*kill abort, retry, fail messages*/
  setupcga(BOOT);          /*display boot screen*/
  sync = NSYNC;            /*set sync to none*/
  dcbuff = _fmalloc(2051); /*allocate data computer xfer buffer*/
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
  game.number = 1; /*initialize game #*/
  if (curconfig.dc == TRUE) {
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
  sent = TRUE;         /*default sent to be true*/
  do                   /*game loop*/
  {
    SetupMonoIdle(1); /*resetup monitor screen*/
    info("<<<< NEW GAME >>>>");
    info(" ");
    info("Sending track to CD & Effects");
    SelectTrack(0); /*pick game track*/
    info("-SENT PRINT TO DC");
    charout(DC, 0xE7);
    charout(DC, 0xE4); /*SEND TO DC GO-AND-PRINT*/
    WaitAck(DC, 4);
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
      vStatLine("STRATEGIC SUMMARY", 17, COLOR(HWHT, BLU), 1);
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
      } else
        setupcga(NOXFER);
    }
  } while (quit == FALSE);
  ResetIBM(); /*Reset timer, close files, etc*/
  return (0);
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
  tokn = TRUE;  /*tokn screen has not been displayed yet*/
  bye = FALSE;  /*default, not ctrl_q'd*/
  sync = PSYNC; /*set sync to PSYNC*/
  recd = 0;     /*init. receive character buffers*/
  recd1 = 0;

  if (curconfig.pc == TRUE) charout(PC, 0xE7);
  for (l = 0; l < 40; ++l) /*clear player records from prev. game*/
  {
    player[l].used = FALSE;
    for (n = 0; n < 10; ++n) {
      player[l].passport[n] = 32;
      player[l].name[n] = 32;
    }
    player[l].score = 0;
  }
  game.mode1 = PUBLIC;
  game.field = 1;
  player[40].score = 0;
  player[41].score = 0;
  game.length = 360;

  for (n = 0; n < 10; ++n) {
    game.redtm1[n] = 32;
    game.redtm2[n] = 32;
    game.grntm1[n] = 32;
    game.grntm2[n] = 32;
  }

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

  while (((Eta < 4) || (Etb < 4)) &
         (bye == FALSE)) /*if both haven't started or ctrl_q'd*/
  {
    if (kbhit()) /*check keyboard*/
    {
      character = getch();
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
    case 0xEE:
      return 1;
      break;
    default:
      return 0;
  }
}

void rungame(void) {
  int bs, l, n;
  int pl = 0;
  int sbegin, send;
  char tempst[40], ts[80];
  char gotch;
  byte view = 0;
  byte cdf = FALSE;
  byte pcs = FALSE;
  info("GAME START Received"); /*acknowledge game start*/
  red_down = 0;                /*clear down slots*/
  players[RED][0] = 0;         /*clear player records*/
  players[RED][1] = 0;
  players[GREEN][0] = 0;
  players[GREEN][1] = 0;
  for (l = 0; l < 65; ++l) npoll[l] = 0;
  if (game.mode1 == PUBLIC) /*find beginning score for field A*/
    bs = curconfig.pub_begin;
  if (game.mode1 == FREEFORALL) bs = curconfig.ffa_begin;
  if (game.mode1 == LEAGUE) bs = curconfig.lea_begin;
  beginscore = bs;
  player[41].used = TRUE; /*initialize test slots*/
  player[42].used = TRUE;
  player[41].score = bs;
  player[42].score = bs;
  vPage(0);
  for (l = 0; l < 40; ++l) rankplayer[l] = NULL;
  if (game.field == 1) /*fill game for one field*/
  {
    for (l = 0; l < 20; ++l) /*remember*/
    {
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
        /* player[l+1].name[l] = '%'; */
      }
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

        /* player[l+21].name[l] = '%'; */
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
  if (game.field == 1) /*set location for status line*/
    stl = 24;
  if (game.field == 2) stl = 12;
  setupcga(ALRTSCRN); /*display blinking alert*/
  poll_flag = FALSE;  /*sync poll() for timing*/
  while (poll_flag == FALSE)
    ;
  timeremain = 29; /*28 seconds...*/
  TCEnable = TRUE;
  info("Sent start to EFFECTS"); /*start effects computer*/
  effectsout("\r", 1);
  sync = PSYNC; /*sync better be PSYNC*/
  if (curconfig.pc == TRUE) charout(PC, 0xE2);
  while (timeremain > 0) {
    if (poll_flag == TRUE) /*Every poll...*/
    {
      poll_flag = FALSE;
      vChangeAttr(COLOR(BLK, WHT));
      vPage(0);
      sprintf(ts, "%2d", timeremain);
      vStatLine(ts, 34, COLOR(HWHT, BLU), 0);
      vPage(1);
      if ((timeremain <= 25) && (pcs == FALSE) && (curconfig.pc == TRUE)) {
        info("Sending data to PC");
        for (l = 0; l < 40; ++l) {
          for (n = 0; n < 10; ++n) charout(PC, player[l + 1].name[n]);
          charout(PC, (byte)player[l + 1].used);
        }
        pcs = TRUE;
      }
      if ((timeremain <= 19) && (cdf == FALSE)) /*time to start cd*/
      {
        cdf = TRUE;
        CD_pause(); /*unpause CD*/
        info("Sent start to CD");
        if (curconfig.pc == TRUE) charout(PC, 0xE3); /*Time to start pc*/
      }
    }
  }
  if (curconfig.et1 != 0) /*Let the ET's go...*/
    outp(ET1, STARTACKN);
  if (curconfig.et2 != 0) outp(ET2, STARTACKN);
  clrcga();
  sync = GSYNC; /*it's GSYNC TIME!*/
  poll_on = 1;
  SetupMonoGame(); /*setup monitor for game*/
  if (game.field == 1) {
    setupcga(GAME1); /*single field game*/
    stl = 24;        /*set status line*/
  }

  else {
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
  timeremain = game.length;           /*set game length*/
  sprintf(tempst, "%4d", timeremain); /*print time remaining*/
  vPage(0);
  vStatLine(tempst, 35, COLOR(HWHT, BLU), 0);
  vPage(1);
  UpdateSync(7, 21, sync); /*display sync set on monitor*/
  UpdateView(0);
  for (; timeremain > -3;) {
    if (poll_flag == TRUE) /*every poll...*/
    {
      poll_flag = FALSE;
      sbegin = slot;
      if (timeremain <= 0) {
        sync = ESYNC;
        UpdateSync(7, 21, sync);
        vPage(0);
        vStatLine("<<<  TERMINATED  >>> ", 18, COLOR(HWHT, HBLU), 0);
        vPage(1);
      }
      if (curconfig.pc == TRUE) outp(PC, 0xE4);
      if (kbhit()) /*check keyboard*/
      {
        gotch = getch();
        if (gotch == password[0])
          pl = 1;
        else if (password[pl] == gotch)
          if (pl < 10) ++pl;
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
          case 'n': /*NO VIEW*/
            view = 0;
            break;
          case 'p': /*REAL TIME POD PERFORMANCE*/ /*RED TEAM*/
            view = 1;
            break;
          case 'P': /*REAL TIME POD PERFORMANCE*/ /*GRN TEAM*/
            view = 6;
            break;
          case 's': /*VIEW OF SCORES AND CODENAMES*/
            view = 3;
            break;
          case 'c': /*VIEW OF PASSPORTS BY SLOT*/
            view = 5;
            break;
        }
      }
      if (curconfig.pc == TRUE) {
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
      }
      charout(PC, 0xE5);
      for (l = 1; l < 41; ++l) /*check pods for resets*/
        if (pod[l].missinrow > 30) pod[l].resetflag = '*';
      /*update view used to be here!!*/
      sprintf(tempst, "%4d", timeremain); /*print time remaining*/
      if (timeremain > 0) {
        vPage(0);
        vStatLine(tempst, 35, COLOR(HWHT, BLU), 0);
        vPage(1);
      }
      PrintMono(6, 21, tempst);
      doscores();                               /*sort & display scores*/
      sprintf(tempst, "%5d", player[41].score); /*display test slot's scores*/
      PrintMono(5, 41, tempst);
      sprintf(tempst, "%5d", player[42].score);
      PrintMono(6, 41, tempst);
      send = slot; /*make note of ending slot*/
      sprintf(tempst, "%2d-%02d-%02d      %2d:%02d:%02d", curdate.month,
              curdate.day, curdate.year, curtime.hour, curtime.minute,
              curtime.second);
      PrintMono(3, 25, tempst);
      vPage(0);
      vChangeAttr(COLOR(HWHT, BLU));
      vPosCur(18, 23);
      v_printf("   %2d:%02d - %2d-%02d-%04d ", curtime.hour, curtime.minute,
               curdate.month, curdate.day, curdate.year);
      vPage(1);
      sprintf(tempst, "BS:%3d ES:%3d RB:%d", sbegin, send,
              badradio); /*print beginning/ending slots*/
      PrintMono(10, 1, tempst);
      UpdateView(view); /*update 'view' screen*/
    }
  }
  view = 0;     /*clear view*/
  sync = ESYNC; /*reset pods*/
  poll_on = 0;
  if ((curconfig.pc == TRUE) && (curconfig.pcmode >= 1))
    if (((game.mode1 == LEAGUE) || (game.mode2 == LEAGUE)) ||
        (curconfig.pcmode == 2)) {
      info("ENQ");
      charout(PC, 0xE6); /*Enquire tournament mode?*/
    }
  UpdateSync(7, 21, sync); /*update sync*/
  vPage(0);
  vStatLine("<<<  TERMINATED  >>> ", 18, COLOR(HWHT, HBLU), 0);
  vPage(1);
  doscores();      /*redisplay scores (one last time*/
  timeremain = 17; /*18 secs of esync*/

  WaitPoll();
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
  }           /*timer count code for e-sync*/
  CD_pause(); /*pause CD player*/
  if (curconfig.pc == TRUE) charout(PC, 0xE7);
  TCEnable = FALSE;
  sync = NSYNC;  /*stop sync*/
  ++game.number; /*increment game #*/
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
    if ((slot > 24) & (slot < 30)) {
      if (red_down > 0) {
        txval = putdown(RED);
        outp(RADIO, txval);
        npoll[slot] = txval;
      } else
        npoll[slot] = 0;
    } else if (slot == 30)
      npoll[slot] = inp(RADIO);
    else if ((slot > 30) & (slot < 51)) {
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
    } else if ((slot > 54) & (slot < 60)) {
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
  /*if (red_down > 30)
           red_down = 30;*/
  for (pd_k = red_down; pd_k > 1; --pd_k) down[pd_k - 1] = (down[pd_k]);
  --red_down;
  return (pd_j);
}

void doscores(void) {
  int tsr, tsg, thitsr, tgethitr, thitownr, thitsg, tgethitg, thitowng;
  volatile struct playertype *temp;
  int j, k, i;
  vPage(0);
  /*Sort the score pointers*/
  /*For ONE field*/
  tsr = tsg = thitsr = tgethitr = thitownr = thitsg = tgethitg = thitowng = 0;
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
  /*Display the scores*/
  /*For BETA setup*/
  tsr = 0;
  tsg = 0;
  if (game.field == 1) {
    if ((curconfig.newscr > 1) ||
        ((curconfig.newscr == 1) && (game.mode1 != PUBLIC))) {
      /*****TOTALLY COOL SCREEN MODE****/
      for (i = 1; i <= players[RED][0]; ++i) {
        if ((i == 1) && ((rankplayer[21] == NULL) ||
                         (rankplayer[1]->score > rankplayer[21]->score)))
          vChangeAttr(COLOR(CRED, HBLK));
        else
          vChangeAttr(COLOR(CRED, BLK));

        vPosCur(0, i + 1);
        v_printf("%s%-10s", rankplayer[i]->baseflag ? "*" : " ",
                 rankplayer[i]->name);
        vChangeAttr(COLOR(CRED, BLK));
        vPosCur(18, i + 1);
        v_printf("%4d %4d %4d %6d", rankplayer[i]->hits, rankplayer[i]->gethit,
                 rankplayer[i]->hitown, rankplayer[i]->score);
        thitsr += rankplayer[i]->hits;
        tgethitr += rankplayer[i]->gethit;
        thitownr += rankplayer[i]->hitown;
        tsr += rankplayer[i]->score;
      }
      for (i = 1; i <= players[GREEN][0]; ++i) {
        /***grn player scores, etc.***/ /**remember, the guy might be flashing*/
        if ((i == 1) && ((rankplayer[1] == NULL) ||
                         (rankplayer[1]->score < rankplayer[21]->score)))
          vChangeAttr(COLOR(GRN, HBLK));
        else
          vChangeAttr(COLOR(GRN, BLK));

        vPosCur(0, i + 12);
        v_printf("%s%-10s", rankplayer[i + 20]->baseflag ? "*" : " ",
                 rankplayer[i + 20]->name);
        vChangeAttr(COLOR(GRN, BLK));
        vPosCur(18, i + 12);
        v_printf("%4d %4d %4d %6d", rankplayer[i + 20]->hits,
                 rankplayer[i + 20]->gethit, rankplayer[i + 20]->hitown,
                 rankplayer[i + 20]->score);
        thitsg += rankplayer[i + 20]->hits;
        tgethitg += rankplayer[i + 20]->gethit;
        thitowng += rankplayer[i + 20]->hitown;
        tsg += rankplayer[i + 20]->score;
      }
      vChangeAttr(COLOR(HWHT, CRED));
      vPosCur(18, 1);
      v_printf("%4d %4d %4d ", thitsr, tgethitr, thitownr);
      vPosCur(33, 1);
      if (tsr > tsg) vChangeAttr(COLOR(HWHT, HRED));
      v_printf("%6d", tsr);
      vChangeAttr(COLOR(HWHT, GRN));
      vPosCur(18, 12);
      v_printf("%4d %4d %4d ", thitsg, tgethitg, thitowng);
      if (tsg > tsr) vChangeAttr(COLOR(HWHT, HGRN));
      vPosCur(33, 12);
      v_printf("%6d", tsg);
    } else {
      for (i = 1; i <= players[RED][0]; ++i) {
        if ((i == 1) && (rankplayer[1]->score > rankplayer[21]->score))
          vChangeAttr(COLOR(CRED, HBLK));
        else
          vChangeAttr(COLOR(CRED, BLK));

        vPosCur(2, i + 1);
        v_printf("%s %-10s", rankplayer[i]->baseflag ? "*" : " ",
                 rankplayer[i]->name);
        vChangeAttr(COLOR(CRED, BLK));
        vPosCur(24, i + 1);
        v_printf("%5d   %6d", rankplayer[i]->hits, rankplayer[i]->score);
        thitsr += rankplayer[i]->hits;
        tgethitr += rankplayer[i]->gethit;
        thitownr += rankplayer[i]->hitown;
        tsr += rankplayer[i]->score;
      }
      for (i = 1; i <= players[GREEN][0]; ++i) {
        /***grn player scores, etc.***/ /**remember, the guy might be flashing*/
        if ((i == 1) && (rankplayer[1]->score < rankplayer[21]->score))
          vChangeAttr(COLOR(GRN, HBLK));
        else
          vChangeAttr(COLOR(GRN, BLK));

        vPosCur(2, i + 12);
        v_printf("%s %-10s", rankplayer[i + 20]->baseflag ? "*" : " ",
                 rankplayer[i + 20]->name);
        vChangeAttr(COLOR(GRN, BLK));
        vPosCur(24, i + 12);
        v_printf("%5d   %6d", rankplayer[i + 20]->hits,
                 rankplayer[i + 20]->score);
        thitsg += rankplayer[i + 20]->hits;
        tgethitg += rankplayer[i + 20]->gethit;
        thitowng += rankplayer[i + 20]->hitown;
        tsg += rankplayer[i + 20]->score;
      }
      vChangeAttr(COLOR(HWHT, CRED));
      vPosCur(24, 1);
      v_printf("%5d", thitsr);
      vPosCur(32, 1);
      if (tsr > tsg) vChangeAttr(COLOR(HWHT, HRED));
      v_printf("%6d", tsr);
      vChangeAttr(COLOR(HWHT, GRN));
      vPosCur(24, 12);
      v_printf("%5d", thitsg);
      if (tsg > tsr) vChangeAttr(COLOR(HWHT, HGRN));
      vPosCur(32, 12);
      v_printf("%6d", tsg);
    }
  }

  /*For ALPHA/OMEGA setup*/
  if (game.field == 2) {
    /*omar*/
  }
  vPage(1);
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
    vPage(0);
    vStatLine("Completed", 15, COLOR(HWHT, BLU), 1);
    vPage(1);
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
void WaitAPoll(void) /*Waits 10 slots...or whatever is after the %*/
{
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
      fwrite(&player[i].passport, sizeof(char), 10, gd);
      fwrite(&player[i].name, sizeof(char), 10, gd);
      fwrite(&player[i].score, sizeof(int), 1, gd);
    }
    fclose(gd);
  } else
    info("! ERROR WRITING GMDATA");
}

void validate(char *filename, unsigned int checksum) {
  FILE *fp;
  unsigned int newsum = 0;
  unsigned char info;
  if ((fp = fopen(filename, "rb")) == NULL) {
    printf("FAIL ON VALIDATE ERROR#01");
    exit(1);
  } else {
    while (!eof(fp)) {
      fread(&info, 1, 1, fp);
      newsum = (newsum + info) % 65535;
    }
    fclose(fp);
  }
  if (newsum != checksum) {
    printf("FAIL ON VALIDATE ERROR#02");
    exit(2);
  }
  printf("ok\n");
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

