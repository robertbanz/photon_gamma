/***********
 glib.c
 ***********/
#define SYSTEM_PROGRAM

#include "glib.h"
#include <conio.h>
#include <dos.h>
#include "gamma.h"

void _far hardhandler(unsigned deverr, unsigned doserr, unsigned far *hdr) {
  _hardretn(doserr);
}

void charout(unsigned int port, byte data) {
  inp(port);
  outp(port, data);
  while ((inp(port + 5) & 0x20) != 0x20)
    ;
}

void PrintMono(int y, int x, char *ts) {
  while (*ts) mono(0, y, x++) = *(ts++);
}
