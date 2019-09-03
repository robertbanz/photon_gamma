/***********
 gamma.h
************/
#include <dos.h>

#define SYSTEMVERSION "1.9"
#define SYSTEMNAME "GAMMA"
#define SYSTEMDATE "8/5/93"

#define STORENUM "010" /*3*/ /*the photon store #*/
#define SITENUM "1" /*2*/    /*what location # for that store*/
#define OPTNUM                                                             \
  "E2" /*1*/ /*02 means two field, 03 means extended ET, E* means extended \
                screens*/
#define LICNUM "A001" /*4*/     /*precursor A means temp, no money*/
#define REVNUM "083092" /*6*/   /*date of last revision*/
#define LOCNAME "010 BALTIMORE" /*pretty simple, really!*/

/*Clock information*/
#define POLLCLOCKLSB
#define POLLCLOCKMSB
#define NORMCLOCKLSB 0x00
#define NORMCLOCKMSB 0x00

/*CTRADIO information*/
#define NSYNC 0x00
#define PSYNC 0xC5
#define GSYNC 0xCA
#define ESYNC 0xDD
#define REDBASE 0x35
#define GRNBASE 0x2B

/*ET information*/
#define ETINTDELAY 30000
#define ETPOLL 0xd2
#define BETA_BOTH_RED 0x13
#define BETA_BOTH_GRN 0x23
#define BETA_RED 0x32
#define BETA_GRN 0x31
#define ALPHA 0x36
#define OMEGA 0x35
#define ETINIT 0xd0
#define ETEND 0x06
#define SENDRED 0xda
#define SENDGRN 0xdb
#define MAINMENU 0xf2
#define EDITMODE 0xf3
#define T_E_M 0xf6
#define S_E 0xf7
#define C_E_M 0xf8
#define WAITGC 0xf9
#define REQXFER 0xfa
#define START 0xfd
#define STARTACKN 0xdd
#define ENDXFER 0xee

/*Hostess information*/
#define BASEPORT 0x240
#define RADIO 0x240
#define ET1 0x248
#define ET2 0x250
#define DC /*0x258*/ 0x3f8
#define EC 0x260
#define PC 0x268
#define PS 0x270
#define LSB9600 0xc
#define MSB9600 0x0
#define LSB1200 0x60
#define MSB1200 0x00
#define SETBAUD 128
#define SEND8n1 3
#define INTRECEIVE 1
#define INTSEND 2
#define INTRECSEND 3

/*Other defines*/
#undef RED
#define RED 1
#define GREEN 2
#define BOTH 4
#define TEST 3
#define FALSE 0
#define TRUE 1
#define PHASERSYNC 0xCA
#define RANDOM 1
#define ORDERED 2
#define TEXT 1
#define GRAPHIC 2
#define CGAMEM ((VIDMEM)(0xB800L << 16)) /*CGA,EGA*/
#define MONMEM ((MEM)(0xB000L << 16))    /*Monochrome*/
#define QUICK 0                          /*Quick screen change*/
#define VERTWIPE 1                       /*Vertical Wipe UP*/
#define DIAGWIPE 2                       /*Diagonal Wipe */
#define RANDOMWIPE 3                     /*Random display*/
#define keyup() outp(RADIO + 4, 3)
#define keydown() outp(RADIO + 4, 0)
#define OFF 0

/*SCREEN TYPES*/
#define TWEENTEXT 1
#define NOXFER 2
#define TWEENGRPH 3
#define GAME1 4
#define GAME2 5
#define INSTOKN 6
#define ALRTSCRN 7

/*GRAPHIC ATTRIBUTES*/
#define REDAb 0x40
#define BLUEAb 0x10
#define GRNAb 0x20
#define INTA 0x00
#define REDA 0x04 | 0x00
#define BLUEA 0x01 | 0x00
#define GRNA 0x02 | 0x00
#define BLINKA 0x80

#define BOOT 3
#define ON 1
#define processpoll(x, y, z) \
  {                          \
    slotnum = x;             \
    value = y;               \
    color = z;               \
    processpoll_f();         \
  }
#define MAXDSLOTS 20
#define fields game.field
#define getrandom(min, max) ((rand() % (int)((max) - (min))) + (min) + 1)
#define WaitPoll()           \
  {                          \
    wpvar = slot;            \
    do {                     \
    } while (slot == wpvar); \
    do {                     \
    } while (slot != wpvar); \
  }
#define ReadPort(x, y)    \
  {                       \
    incoming = (byte *)y; \
    *incoming = inp(x);   \
  }

#define PUBLIC 1
#define LEAGUE 2
#define FREEFORALL 3
#define FREEFORALLPS 4

/*Typedefs*/
typedef unsigned short far *VIDMEM;
typedef unsigned char byte;
typedef unsigned char far *MEM;

/*Macro*/
#define mono(x, y, z) *(monscreen + ((z)*2) + (x) + ((y)*160))
#define NORMAL 0x07
#define VIDREV 0x70
#define BLINK 0x80
#define INTENSE 0x08

#pragma pack(1)
struct config {
  byte et1, et2, manuever, pc, dc, ec, ps, cdsel;
  byte field, players, curcd, tween;
  int pub_begin, lea_begin, ffa_begin;
  byte pub_hcp;
  byte lea_hcp;
  byte ffa_hcp;
  char alpha_red[10], alpha_grn[10];
  char omega_red[10], omega_grn[10];
  char beta_red[10], beta_grn[10];
  int length;
  byte vidmode;
  char mess_path[10], pod_path[10], data_path[10], log_path[10];
  int cdtype;
  int etfake;
  byte pcmode;
  int savedata;
  byte newscr;
};
#pragma pack()

struct playertype {
  char name[10];
  char passport[10];
  int score;
  unsigned char baseflag;
  unsigned char used;
  unsigned char handihits;
  int hits, gethit, hitown; /* Those fuckers want OOPS, but there's no way in
                               hell they're gonna get it!*/
};

struct gamma_current {
  int game_number;
  struct playertype highscore;
  struct playertype highhits;
};

struct pc_playerinfo {
  char name[10];
  int hitopp[40];
  char baseflag;
  char used;
  char reset;
};

#pragma pack(1)
struct gamestruct {
  char redtm1[10], grntm1[10], redtm2[10], grntm2[10];
  byte field;
  byte mode1, mode2;
  byte hcp_a, hcp_b;
  int number;
  int length;
};

struct podstruct {
  int id, base, rxmiss, rxbad, txmiss, txbad, valcode, hitackn, hitsent;
  byte podid;
  byte missinrow;
  byte resetflag;
};

struct gamerec {
  struct dostime_t curtime;
  struct dosdate_t curdate;
  struct {
    int score;
    struct podstruct pod;
  } sp[40];
  struct gamestruct game;
  struct pc_playerinfo player[40];
  byte GameData[360][40];
};

#pragma pack()

struct com_5 {
  unsigned DR : 1;
  unsigned OE : 1;
  unsigned PE : 1;
  unsigned FE : 1;
  unsigned BI : 1;
  unsigned THRE : 1;
  unsigned TSRE : 1;
  unsigned NOT : 1;
};

struct com_3 {
  unsigned WSL0 : 1;
  unsigned WSL1 : 1;
  unsigned STB : 1;
  unsigned PEN : 1;
  unsigned EPS : 1;
  unsigned STP : 1;
  unsigned SBK : 1;
  unsigned DLAB : 1;
};

struct com_2 {
  unsigned IP : 1;
  unsigned IID0 : 1;
  unsigned IID1 : 1;
  unsigned x0 : 1;
  unsigned x1 : 1;
  unsigned x2 : 1;
  unsigned x3 : 1;
  unsigned x4 : 1;
};

struct com_1 {
  unsigned E_DA : 1;
  unsigned E_THRE : 1;
  unsigned E_RS : 1;
  unsigned E_MS : 1;
  unsigned x0 : 1;
  unsigned x1 : 1;
  unsigned x2 : 1;
  unsigned x3 : 1;
};

struct com_4 {
  unsigned DTR : 1;
  unsigned RTS : 1;
  unsigned OUT1 : 1;
  unsigned OUT2 : 1;
  unsigned LOOP : 1;
  unsigned x0 : 1;
  unsigned x1 : 1;
  unsigned x2 : 1;
};

struct com_6 {
  unsigned DCTS : 1;
  unsigned DDSR : 1;
  unsigned TERI : 1;
  unsigned DRLSD : 1;
  unsigned CTS : 1;
  unsigned DSR : 1;
  unsigned RI : 1;
  unsigned RLSD : 1;
};
#pragma pack(1)
struct time {
  byte minute;
  byte hour;
  byte hsecond;
  byte second;
};

struct date {
  int year;
  byte day;
  byte month;
};
#pragma pack()

#ifdef SYSTEM_PROGRAM
#include "photon.h"
#endif

#include "phoproto.h"

