#include <stdio.h>
#include "video.h"

void splash(char *dah) {
  while (*dah) vPutch(*dah++);
}

int main(void) {
  int rval;
  rval = ASDInit(FINDADAPTER);
  splash("This is the default screen...");
  vChangeAttr(COLOR(HWHT, BLU));
  vPosCur(5, 2);
  vBox(CHARMASK | ATTRMASK, '#', 30, 10);
  vStatLine(" This is on the primary color display", 0, COLOR(BLU, HWHT), 1);
  vPage(1);
  vStatLine(" This is on the secondary status line... ooooh", 0,
            COLOR(BLK, WHT), 1);
  splash("This is the secondary screen ...");
  vPosCur(20, 5);
  vBox(CHARMASK | ATTRMASK, '*', 20, 15);
  vGetImage(30, 10, 1);
  vPosCur(25, 10);
  vPutImage(1);
  getch();
  ASDDeInit();
}
