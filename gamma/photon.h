/***********
 photon.h
************/

#ifdef __TOPLEV
#define DCL
#endif
#ifndef __TOPLEV
#define DCL extern
#endif

DCL char huge *appstack;    /*stack pointer during interrupt*/
DCL struct gamestruct game; /*information on game*/
volatile DCL struct playertype
    *rankplayer[41];         /*indexed array of pointers to players*/
DCL byte transfer[2][600];   /*buffer for incoming transfer*/
DCL byte xx, yy, zz;         /*GLOBALS used to pass from poll to processpoll*/
DCL byte i;                  /*standard counter*/
DCL byte sync;               /*current sync byte*/
DCL byte poll_val;           /*Temp. variable used in poll()*/
volatile DCL byte npoll[70]; /*rx/tx values*/
volatile DCL byte slot;      /*current slot# poll() is polling*/
DCL byte txval;              /*value to be transmitted by poll()*/
DCL byte red_down;           /*amt of people in down slots*/
volatile DCL byte poll_flag; /*indicates poll data ready to be read*/
volatile DCL byte tickcount; /*keeps track of ticks for real time clock*/
DCL int intno;               /*interuppt number*/
DCL union REGS reg;          /*register values on call*/
DCL int irslot[21][4];       /*xref table player->ircodes*/
DCL int slotir[0xFF];        /*xref table ircode to player*/
DCL struct config curconfig, newconfig;    /*current/new system configuration*/
volatile DCL struct playertype player[43]; /*player information*/
DCL byte cga1[2][25][41];                  /*CGA page*/
DCL byte down[50];                         /*down slots*/
DCL byte hcp;                              /*Handicapped mode ?*/
DCL int beginscore;                        /*beginning score*/
DCL char j;                                /*plain global variable*/
DCL byte pd_j, pd_k; /*Variables used in PutDown routine...must be global*/
DCL byte slotnum, value, color, codeflag; /*used by processpoll*/
DCL byte players[3][2];                   /*# of players on each team*/
DCL byte stl;                             /*status line location*/
DCL byte sent;                            /*TRUE if data comp. rec'd xfer*/
DCL byte gametrack;                       /*game track #*/
DCL MEM monscreen;                        /*base address for monochrome screen*/
DCL byte *track_lut;                      /*random track table*/
DCL char textinfo[10]
                 [40]; /*holds text info between et code & field start screens*/
DCL byte wpvar;        /*used in WaitPoll macro*/
DCL byte idsig[44];    /*xref table id#->rom signatures*/
DCL byte sigid[256];   /*xref table rom signatures->id#*/
DCL struct podstruct pod[44]; /*pod performance data*/
DCL char *buffer;             /*buffer to hold data computer information*/
DCL void far *dcbuff;
DCL struct dostime_t curtime; /*current time*/
DCL struct dosdate_t curdate; /*current date*/
DCL byte ctrl_e;              /*TRUE if prev. game was aborted*/
DCL byte *time_info;          /*time array (adjust to exact time*/
DCL byte time_info_num;       /*which element of the time array is being used*/
DCL int badradio;             /*# 0x39's received by the radio*/
DCL byte *incoming;           /*pointer used in ReadPort macro*/
DCL int seconds, minutes, hours; /*used in IncTime() funciton*/
DCL int tnum;                    /*used in GAMMA2.C somewhere*/
DCL byte TCEnable;               /*Enables poll to count down the timer*/
DCL char password[10];
DCL byte gameslot;
volatile DCL int timeremain;
DCL byte poll_on;
