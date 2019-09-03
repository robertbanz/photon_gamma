/***********
 sethost.c
************/

#include <conio.h>
#include <dos.h>
#include "gamma.h"

void setuphostess() {
  outp(RADIO + 3, 128);
  outp(RADIO, 0x60);
  outp(RADIO + 1, 0x00);
  outp(RADIO + 3, 3);
  outp(RADIO + 1, 1);
  outp(RADIO + 7, 255);
  outp(RADIO + 4, 2);

  outp(ET1 + 3, SETBAUD);
  outp(ET1, LSB9600);
  outp(ET1 + 1, MSB9600);
  outp(ET1 + 3, SEND8n1);
  outp(ET1 + 1, 1);
  outp(ET1 + 4, 3);

  outp(ET2 + 3, SETBAUD);
  outp(ET2, LSB9600);
  outp(ET2 + 1, MSB9600);
  outp(ET2 + 3, SEND8n1);
  outp(ET2 + 1, 1);
  outp(ET2 + 4, 3);

  /*4800 BAUD*/
  outp(DC + 3, SETBAUD);
  outp(DC, LSB9600); /*0x30*/
  outp(DC + 1, MSB9600);
  outp(DC + 3, SEND8n1);
  outp(DC + 1, 1);

  outp(PC + 3, SETBAUD);
  outp(PC, LSB9600);
  outp(PC + 1, MSB9600);
  outp(PC + 3, SEND8n1);
  outp(PC + 1, INTRECEIVE);

  outp(PS + 3, SETBAUD);
  outp(PS, LSB9600);
  outp(PS + 1, MSB9600);
  outp(PS + 3, SEND8n1);
  outp(PS + 1, INTRECSEND);

  outp(EC + 3, SETBAUD);
  outp(EC, LSB9600);
  outp(EC + 1, MSB9600);
  outp(EC + 3, SEND8n1);
  outp(EC + 1, INTRECSEND);
}

