/**********
 phoproto.h
 **********/

/******************************************
 *   DEFINED IN GAMMA.C                    *
 *****************************************/
/***********************************
 * DEFINED IN GAMMA2.C              *
 ***********************************/
#include "gamma2.h"

/**************************************
 * DEFINED IN GAMMA3.C                 *
 *************************************/

void UpdateView(byte);
void DispGameModes(void);
void DispGameModes2(int, int, char *, char *);
void SetupMonoGame(void);
void SetupMonoIdle(byte);
void box2(int, int, int, int);
void info(char *);
void UpdateMonoIdle(byte, byte, byte);
void PrintEtStatus(int, int, byte);
void UpdateSync(int, int, byte);
void CheckPassport(char *);
void NewTrack(byte);

/****************************************
 * DEFINED IN CONFIGSY.C                 *
 ****************************************/

void setupmenu(void);

/****************************************
 * DEFINED IN SETUP.C                    *
 ****************************************/

void sys_config(byte);
void ConfigEts(byte);
void FatalError(char *);
void ET_config1(byte);
void ET_config2(byte);

/****************************************
 * DEFINED IN SETHOST.C                  *
 ****************************************/

void setuphostess(void);

/****************************************
 * DEFINED IN GLIB.C                     *
 ****************************************/

void _far hardhandler(unsigned, unsigned, unsigned far *);
void charout(unsigned, byte);
void PrintMono(int, int, char *);
void box1(int, int, int, int);

/****************************************
 * DEFINED IN PODPERF.C                  *
 ****************************************/

void SetupPodId(void);
byte IsId(byte);

/****************************************
 * DEFINED IN CD.C                       *
 ****************************************/

void CD_sys(byte);
void CD_track(byte);
void CD_play(void);
void CD_pause(void);
void CD_clear(void);
void CD_stop(void);
void CD_time(void);
void CD_reset(void);
