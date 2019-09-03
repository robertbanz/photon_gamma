/***********
 glib.c
 ***********/

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
  while (*(ts) != '\0') {
    mono(0, y, x++) = *(ts++);
  }
}

void box1(int x, int y, int i, int j) {
  int l, m;
  char tr = '¿';
  char tl = 'Ú';
  char br = 'Ù';
  char bl = 'À';
  char hr = 'Ä';
  char vr = '³';
  for (l = x; l <= i; ++l) {
    mono(0, y, l) = hr;
    mono(0, j, l) = hr;
  }
  for (l = y; l <= j; ++l) {
    mono(0, l, x) = vr;
    mono(0, l, i) = vr;
  }
  mono(0, y, x) = tl;
  mono(0, j, i) = br;
  mono(0, j, x) = bl;
  mono(0, y, i) = tr;
}
