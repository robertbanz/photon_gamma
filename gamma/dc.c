/**************
  dc.c
***************/

#include "gamma.h"
#define HOST1 0x240
#define COM2 0x2f8
#define GC ioport
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\comm\comm.h"

byte cga1[2][25][41];

void initdc(void);
void dispcgaline(int);
void PrintOut(void);

main(int argc, char *argv[]) {
  /* init io ports */
  ASYNC *port;
  FILE *printer;
  char count = FALSE;
  char ts[41];
  char printall;
  char *buffer;
  MEM omar;
  byte printon;
  int l, j, k, m;
  int x, y;
  char ch;
  byte n;
  byte stl;
  byte initflag;
  int ioport;
  outp(0x3d9, 0x11);
  outp(0x3d8, 0x28);

  if ((port = (ASYNC *)malloc(sizeof(ASYNC))) == NULL) {
    printf("Not enough memory\n");
    exit(0);
  }

  if (argc != 3) {
    printf("Too Few/Too Many arguements\n");
    printf("Usage: dc <port type> <port #>\n");
    printf("   Where:  port type is H for hostess\n");
    printf("                        C for COMX\n");
    printf("           port #    is <1-8> for hostess\n");
    printf("                     is <1-2> for COM ports\n");
    exit(0);
  } else {
    if (toupper(argv[1][0]) == 'H')
      ioport = 0x240 + (atoi((argv[2])) * 8) - 8;
    else
      switch (argv[2][0]) {
        case '1':
          ioport = 0x3f8;
          break;
        case '2':
          ioport = 0x2f8;
          break;
        case '3':
          ioport = 0x3e8;
          break;
        case '4':
          ioport = 0x2e8;
          break;
      }
  }
  printf("DC screen server installed on port %X\n\n", ioport);
  CGA_hidecursor();
  clrcga();
  dispcga(1, QUICK);

  AllocRingBuffer(port, 1024, 1024, 0);

  if (async_open(port, ioport, IRQ3, VCTR3, "9600N81") != R_OK) {
    printf("Open Port Failed\n");
    exit(0);
  }

  initflag = FALSE;
  buffer = malloc(2051);
  printon = TRUE;
  while (1 == 1) {
    if (kbhit()) {
      ch = getch();
      switch (ch) {
        case (17):
          exit(-1);
          break;
        case ('p'):
          if (printon == TRUE) PrintOut();
          break;
        case ('P'):
          printall = TRUE;
          break;
        case ('o'):
          printall = FALSE;
          break;
      }
    }
    if (async_rxcnt(port)) {
      n = async_rx(port);
      if (n == 0xF5) {
        initdc();
        l = 0;
        async_tx(port, 0xF5);
      } else if (n == 0xE1) {
        l = 0;

        clrcga();
        dispcga(1, QUICK);
        async_tx(port, 0xE2);
        count = TRUE;

      } else if (n == 0xF7) {
        async_tx(port, 0xF7);
        while (!async_rxcnt(port))
          ;
        n = async_rx(port);
        outp(0x200, n);
        async_tx(port, n);

      } else if (n == 0xE4) {
        if (printall == TRUE) PrintOut();
        async_tx(port, 0xe4);
      } else if (n == 0xE6) {
        printon = FALSE;
        async_tx(port, 0xE6);
      } else if (n == 0xE7) {
        printon = TRUE;
        async_tx(port, 0xE7);
      } else {
        if ((count == TRUE) && ((l % 50) == 0)) {
          sprintf(&cga1[1][23][3], "%4d", l);
          dispcgaline(23);
        }
        buffer[l] = n;
        /*async_tx(port,l);*/
        ++l;
      }
      if (l == 2000) {
        count = FALSE;
        l = 0;
        omar = (MEM)CGAMEM;
        for (l = 0; l < 2000; ++l) *(omar++) = buffer[l];
        l = 0;
        async_tx(port, 0xE3);
      }
    }
  }
  free(buffer);
}

void dispcga(int which, int how) {
  int x, y;
  int j;
  VIDMEM screen;
  j = 40;
  screen = CGAMEM;
  if (which == 1)
    for (y = 0; y < 25; ++y)
      for (x = 0; x < 40; ++x)
        *(screen + (y * j) + x) = (cga1[0][y][x] << 8) | cga1[1][y][x];
}

void dispcgaline(int which) {
  int x;
  VIDMEM screen;
  screen = CGAMEM;
  for (x = 0; x < 40; ++x)
    *(screen + (which * 40) + x) = (cga1[0][which][x] << 8) | cga1[1][which][x];
}
void clrcga(void) {
  int x, y;
  for (y = 0; y < 25; ++y)
    for (x = 0; x < 40; ++x) {
      cga1[0][y][x] = REDA | GRNA | BLUEA;
      cga1[1][y][x] = 32;
    }
}

void CGA_hidecursor(void) {
  outp(0x3d4, 14);
  outp(0x3d5, 0x1f);
  outp(0x3d4, 15);
  outp(0x3d5, 0x41);
}

void initdc(void) {
  int i, j;
  clrcga();
  sprintf(&cga1[1][10][9], "GREETINGS,  EARTHLINGS");
  sprintf(&cga1[1][13][8], "WELCOME TO PLANET PHOTON");
  for (i = 1; i < 40; ++i) {
    cga1[0][10][i] = BLUEA | REDA | GRNA;
    cga1[0][13][i] = BLUEA | REDA | GRNA;
  }
  dispcga(1, QUICK);
}

void PrintOut(void) {
  /* DON'T EVEN THINK ABOUT IT...YET  FILE *printer;
  int x,y;
  if ((inp(0x3bd) == 0xDF) || (inp(0x379) == 0xDF))
  {
          printer=fopen("LPT1","w");

          for (y=0;y<25;++y)
          {
                  for(x=0;x<40;++x)
                          fprintf(printer,"%c",cga1[1][y][x]);
                  fprintf(printer,"\n");
          }
          fprintf(printer,"\f");
          fclose(printer);
  }
*/
}

