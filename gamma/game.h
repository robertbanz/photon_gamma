#ifndef GAME
#define GAME
/*
    Standard Game Data Structures

    Ok...here goes.
    (c) 1993, Robert Banz
*/
#ifndef SCREENS
#include "screens.h"
#endif
#ifndef CONFIG
#include "config.h"
#endif

#pragma pack(1)
struct GameType_t {
  unsigned std_2_team : 1;                        /*public,league,free for all*/
  unsigned hit_own_player : 1; /*public,league*/  /*not with hit_all_players*/
  unsigned hit_base : 1;                          /*public,league*/
  unsigned hit_all_players : 1; /*free for all*/  /*not with hit_own_player*/
  unsigned disrupt_by_base : 1; /*public,league*/ /*only with hit_base*/
  unsigned jeopardy : 1;
      /*public,league,free for all*/ /*not with handi/ext hadi or basejeopardy*/
  unsigned base_jeopardy : 1; /*public,league*/ /*not with jeopardy*/
  unsigned std_handicapping : 1;
      /*public,league,free for all*/ /*not with jeopardy*/
  unsigned extended_handicapping : 1;
      /*public,league,free for all*/            /*not with jeopardy*/
  unsigned base_rush : 1;                       /*public,league*/
  unsigned base_end : 1;                        /*public,league*/
  unsigned reverse_base_rush : 1;               /*public,league*/
  unsigned x_team : 1;                          /*free for all*/
  unsigned whole_team_down : 1;                 /*public,league,free for all*/
  unsigned x_team_ts_only : 1; /*free for all*/ /*with xteam only*/
  unsigned phaser_station : 1;
      /*public,league,free for all*/ /*not with jeopardy or std. hadicapping*/
  unsigned dual_field : 1; /*public,league,free for all*/   /*not with xteam*/
  unsigned large_single : 1; /*public,league,free for all*/ /*not with dual*/
  unsigned forever_length : 1;
      /*public,league,free for all*/   /*not with base end*/
  unsigned down_time_handicapping : 1; /*public*/

  byte amt_to_get_base;
  byte jeopardy_percent;
  byte x_team_amount;
  byte ext_handicapping_percent;
  byte players[2];

  int beginning_score;
  int length;

  char name[41];
}

typedef struct g_player_t {
  char Name[15];
  long PPprefix;
  long PPnumber;
  int score;
  unsigned char base;
  unsigned char used;
  int data1;
  int data2;
  int data3;
  int data4;
  int hits, gethit, hitown;
} player;

typedef struct pod_t {
  int Id,        /*ID count*/
      base,      /*base code count*/
      rxmiss,    /*Pod Receiver miss count*/
      rxbad,     /*Pod Receiver bad count*/
      txmiss,    /*Pod Transmit miss count*/
      txbad,     /*Pod Transmit bad count*/
      valcode,   /*Pod Transmit valid count*/
      hitackn,   /*Hits Acknowleged*/
      hitsent;   /*Hits sent out*/
  byte podid;    /*pod ID*/
  int missinrow; /*#of codes missed in a row*/
  int OutOfGame; /*out of game flag*/
} pod;

typedef struct game_t {
  player Players[40];
  struct GameType_t GameMode;
  int TeamScore[2];
  char TeamName[2][15];
  int TimeRemaining;
  pod Pods[40];
  int Players_per_team[2];
} game;

typedef struct config_t {
  char ScreenDefs[10]; /*file containing screen definitions*/
  int DebugLevel;
  int ETMode[2];
  int SaveGameMode;
  int MaxPerTeam;
  char GameModes[10];  /*file containing game mode defs*/
  char GameTracks[10]; /*file containing game track defs*/
  char Def_TeamName[2][15];
  char game_path[20], perf_path[20], log_path[20];
}
#pragma pack()

typedef struct System_defs {
  /*screens*/
  screen Console, Current, Post, PlaybyPlay;

  /*games*/
  game C_Game, default;

  /*other stuff*/
  config Sys_Config;
} system;

#endif
