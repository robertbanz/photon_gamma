
#include "asd.h"

typedef struct {
  /*this is PARTLY the asdcore structure from asddat.h*/
  int vpage_switch;
  int vpagenum;
  int (*recv)(void),             /* stdin function for menus etc. */
      (*rxstat)(void);           /* stdin status function */
  void (*output)(unsigned char), /* standard output for ASD */
      (*rawout)(unsigned char),  /* raw output (serial etc) */
      (*respnd)(unsigned char),  /* response fn (avoid feedback) */
      (*flow)(unsigned char),    /* flow control function */
      (*ASDcurfn)(void),         /* current function waiting on */
      (*ASDput)(unsigned char);  /* current output function */
  int ASDgmode;                  /*current graphics mode*/
} iodev;

extern iodev CONS;
extern iodev PUB;
extern iodev *CURRENTDEV;

void SelectIO(iodev *device);
void InitIO(void);
int recv_console(void);
int rxstat_console(void);
void rawout_console(unsigned char a);
