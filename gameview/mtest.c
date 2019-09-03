#include <stdio.h>
#include "asd.h"
#include "asdmenu.h"
#include "video.h"

void vUpdateView(void);

/* void _fmemset(char _far *source, char val, int nbytes)
{
        while (nbytes--)
                *source++ = val;
}

void _fmemcpy(char _far *source, char _far *dest, int nbytes)
{
        while (nbytes--)
                *dest++ = *source++;
}

void _fmemmove(char _far *source, char _far *dest, int nbytes)
{
        if (!nbytes)
                return;
        if (source < dest)
                {
                source = &source[nbytes];
                source--;
                dest = &dest[nbytes];
                dest--;
                }
        while (nbytes--)
                *dest-- = *source--;
} */
int main(void) {
  int rval, lp;
  vInit(FINDADAPTER);
  vCursorControl(CUR_NOUPDATE);
  vChangeAttr(COLOR(WHT, BLU));
  vStatLine("vprintf", 10, COLOR(HWHT, HBLK), 1);
  vStatLine(" ready    ", 0, COLOR(HWHT, HBLK), 0);
  if (vGetch() <= 1) vGetch();
  vStatLine(" running  ", 0, COLOR(HWHT, HBLK), 0);
  for (lp = 0; lp < 2000; lp++) vUpdateView();
  vStatLine(" done     ", 0, COLOR(HWHT, HBLK), 0);
  if (vGetch() <= 1) vGetch();
  vDeInit();
}

void vUpdateView(void) {
  unsigned char y;
  vPosCur(0, 0);
  v_sends(" ID -----------³RX STAT³TX STAT³HITS---º");
  vPosCur(0, 1);
  v_sends("  #  ID BSE VAL³RXM RXB³TXM TXB³SNT ACKº");
  for (y = 2; y < 13; ++y) {
    vPosCur(0, y);
    v_sends("               ³       ³       ³       º");
  }
  for (y = 0; y < 10; y++) {
    vPosCur(0, 2 + y);
    v_printf("%c%2d %3d %3d %3d³%3d %3d³%3d %3d³%3d %3dº", ' ', 14, 34, 1, 32,
             2, 3, 0, 1, 34, 30);
  }
}
