/* io device control stuff */
/* **mainly** for switching between our console device and our
   video device NOW. */
#include "outctl.h"
#include "..\comm\comm.h"
#include "asddat.h"
#include "video.h"

iodev CONS;
iodev PUB;
iodev *CURRENTDEV;
ASYNC *CONS_PORT;
extern ASD_COMP ASDcore;

void SelectIO(iodev *device) {
  if (device->vpage_switch) vPage(device->vpagenum);
  CURRENTDEV->output = ASDcore.output;
  ASDcore.output = device->output;
  CURRENTDEV->rxstat = ASDcore.rxstat;
  ASDcore.rxstat = device->rxstat;
  CURRENTDEV->recv = ASDcore.recv;
  ASDcore.recv = device->recv;
  CURRENTDEV->rawout = ASDcore.rawout;
  ASDcore.rawout = device->rawout;
  CURRENTDEV = device;
}

void InitIO(void) {
  CURRENTDEV = &PUB;

  CONS.vpage_switch = 1;
  CONS.vpagenum = 1;
  PUB.vpage_switch = 1;
  PUB.vpagenum = 0;
  /*
      CONS_PORT = (ASYNC *) malloc(sizeof(ASYNC));

      AllocRingBuffer(CONS_PORT , 2048, 2048, 0);

      if (async_open(CONS_PORT, 0x3f8, IRQ4, VCTR4, "56000N81") != R_OK) {
          printf("Open Port Failed\n");
          ResetIBM();
          exit(0);
      }

      async_msrflow(CONS_PORT,0);
      async_xflow(CONS_PORT,0);
      async_rts(CONS_PORT,1);
      async_dtr(CONS_PORT,1);

      CONS.output = vPutch;
      CONS.recv = recv_console;
      CONS.rxstat = rxstat_console;
      CONS.rawout = rawout_console;
  */
  CONS.vpage_switch = 1;
  CONS.vpagenum = 1;
}

int recv_console(void) {
  static char a;
  while (1) {
    if (async_rxcnt(CONS_PORT)) return async_rx(CONS_PORT);
    if (vKbhit()) return vGetch();
  }
}

int rxstat_console(void) {
  if (vKbhit())
    return vKbhit();
  else
    return (async_rxcnt(CONS_PORT) != 0) ? 1 : 0;
}

void rawout_console(unsigned char a) { async_tx(CONS_PORT, a); }
