#include <conio.h>
#include <graph.h>
#include <stdio.h>
#include <stdlib.h>

main() {
  int i;
  char ts[100];
  for (i = 0; i < 0x80; ++i) {
    if (i % 16 == 0) _settextposition((i / 16) * 3 + 1, 0);
    sprintf(ts, "%02x   ", i);
    _outtext(ts);
  }
  while (1 == 1) {
    while (!kbhit())
      ;
    getch();
    for (i = 0; i < 0x80; ++i) {
      outp(0x224, i);
      if (i % 16 == 0)
                                                _settextposition((i / 16) * 3 + 2/////,0);
					sprintf(ts,"%02x   ",inp(0x225));
					_outtext(ts);
    }
  }
}
