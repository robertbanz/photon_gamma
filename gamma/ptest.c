#include <dos.h>
#include <stdlib.h>
typedef unsigned char byte;

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

/*Hostess information*/
#define BASEPORT 0x240
#define RADIO 0x240
#define ET1 0x248
#define ET2 0x250
#define DC 0x258
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

void charout(unsigned int port, byte data) {
  inp(port);
  outp(port, data);
  while ((inp(port + 5) & 0x20) != 0x20)
    ;
}

main() {
  int quit = 0;
  int send = 0;

  outp(PS + 3, SETBAUD);
  outp(PS, LSB9600);
  outp(PS + 1, MSB9600);
  outp(PS + 3, SEND8n1);
  outp(PS + 1, INTRECSEND);

  printf("Press 1 to start sending, 0 to end, 2 to quit");

  while (quit == 0) {
    if (send == 1) charout(PS, 0xCA);
    if (kbhit()) switch (getch()) {
        case '1':
          send = 1;
          printf("sending...\n");
          break;
        case '2':
          quit = 1;
          printf("quiting...\n");
          break;
        case '0':
          send = 0;
          printf(" not sending...\n");
      }
  }
}
