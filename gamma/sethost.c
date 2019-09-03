/***********
 sethost.c
************/

#define SYSTEM_PROGRAM
#include <conio.h>
#include <dos.h>
#include "gamma.h"

void setuphostess() {
  if (RADIO && (RADIO != 0x3f8)) {
    outp(RADIO + 3, 128);
    outp(RADIO, 0x60);
    outp(RADIO + 1, 0x00);
    outp(RADIO + 3, 3);
    outp(RADIO + 1, 1);
    outp(RADIO + 7, 255);
    outp(RADIO + 4, 2);
  }
  if (ET2 && (ET2 != 0x3f8)) {
    outp(ET2 + 3, SETBAUD);
    outp(ET2, LSB9600);
    outp(ET2 + 1, MSB9600);
    outp(ET2 + 3, SEND8n1);
    outp(ET2 + 1, 1);
    outp(ET2 + 4, 3);
  }
  if (DC && (DC != 0x3f8)) {
    outp(DC + 3, SETBAUD);
    outp(DC, LSB9600);
    outp(DC + 1, MSB9600);
    outp(DC + 3, SEND8n1);
    outp(DC + 1, 1);
  }
  if (PC && (PC != 0x3f8)) {
    outp(PC + 3, SETBAUD);
    outp(PC, LSB9600);
    outp(PC + 1, MSB9600);
    outp(PC + 3, SEND8n1);
    outp(PC + 1, INTRECEIVE);
  }
  if (PS && (PS != 0x3f8)) {
    outp(PS + 3, SETBAUD);
    outp(PS, LSB9600);
    outp(PS + 1, MSB9600);
    outp(PS + 3, SEND8n1);
    outp(PS + 1, INTRECSEND);
  }
  if (EC && (EC != 0x3f8)) {
    outp(EC + 3, SETBAUD);
    outp(EC, LSB9600);
    outp(EC + 1, MSB9600);
    outp(EC + 3, SEND8n1);
    outp(EC + 1, INTRECSEND);
  }
  if (ET1 && (ET1 != 0x3f8)) {
    outp(ET1 + 3, SETBAUD);
    outp(ET1, LSB9600);
    outp(ET1 + 1, MSB9600);
    outp(ET1 + 3, SEND8n1);
    outp(ET1 + 1, 1);
    outp(ET1 + 4, 3);
  }
}
