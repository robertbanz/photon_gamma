#include <stdio.h>
#include <stdlib.h>
#include "asd.h"
#include "g_asd.h"

#pragma pack(1)
struct sldhdr_t {
  char id[17];
  unsigned char type;
  unsigned char leve;
  unsigned int xsize;
  unsigned int ysize;
  unsigned long aspect;
  unsigned int hardwarefill;
  unsigned int testnumber;
};
#pragma pack()

unsigned char ColorXref[256] = {0,  4,  14, 2,  3,  1,  5,  15, 8,  7,  /*00*/
                                4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  /*01*/
                                4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  /*02*/
                                6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  /*03*/
                                6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  /*04*/
                                14, 14, 14, 14, 14, 14, 14, 14, 14, 14, /*05*/
                                14, 14, 14, 14, 14, 14, 14, 14, 14, 14, /*06*/
                                2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  /*07*/
                                2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  /*08*/
                                2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  /*09*/
                                3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  /*10*/
                                3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  /*11*/
                                3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  /*12*/
                                3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  /*13*/
                                1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  /*14*/
                                1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  /*15*/
                                1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  /*16*/
                                1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  /*17*/
                                1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  /*18*/
                                1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  /*19*/
                                1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  /*20*/
                                13, 13, 13, 13, 13, 13, 13, 13, 13, 13, /*21*/
                                13, 13, 13, 13, 13, 13, 13, 13, 13, 13, /*22*/
                                13, 13, 13, 13, 13, 13, 13, 13, 13, 13, /*23*/
                                4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  /*24*/
                                8,  8,  8,  7,  7,  7};

union intnbyte {
  signed char c[2];
  unsigned char uc[2];
  int i;
};

int SlideRecRead(union intnbyte *where, int num, FILE *in) {
  return (fread(where, 1, num, in));
}

unsigned char AcadCol2Vga(unsigned char in) { return ColorXref[in]; }

void load_sld(char *fname, int x0, int y0, int x1, int y1, float wx0, float wy0,
              float wx1, float wy1) {
  struct sldhdr_t header;
  void *temp;
  FILE *in;
  int vecinvalid = 1, vecx, vecy, fvecx, fvecy;

  int lastx, lasty;
  int color;

  union intnbyte input[50];
  int innum = 0;

  if ((in = fopen(fname, "rb")) == 0) {
    return;
  }

  fread(&header, 31, 1,
        in); /*read header, we can give a shit about this later*/

  sxSetWindow(1, 0, 0, header.xsize, header.ysize);

  while (!feof(in)) {
    SlideRecRead(&input[0], 2, in);

    if ((input[0].uc[1] <= 0x7F) && (input[0].uc[1] >= 0x00)) {
      /*Vector*/

      SlideRecRead(&input[1], 6, in);
      sxwDrawLine(input[0].i, input[1].i, input[2].i, input[3].i);
      lastx = input[0].i;
      lasty = input[1].i;
    } else if (input[0].uc[1] == 0xfb) {
      /*offset vector*/

      SlideRecRead(&input[1], 3, in);
      sxwDrawLine((input[0].c[0] + lastx), (input[1].c[0] + lasty),
                  (input[1].c[1] + lastx), (input[2].c[0] + lasty));
      lastx += input[0].c[0];
      lasty += input[1].c[0];
    } else if (input[0].uc[1] == 0xfc) {
      return (1); /*end of file*/
    } else if (input[0].uc[1] == 0xfd) {
      SlideRecRead(&input[1], 4, in);
      if (input[2].i < 0) {
        if (vecinvalid)
          vecinvalid = 1;
        else {
          sxwDrawLine(vecx, vecy, fvecx, fvecy);
          vecinvalid = 1;
        }
      } else if (vecinvalid) {
        fvecx = vecx = input[1].i;
        fvecy = vecy = input[2].i;
        vecinvalid = 0;
      } else {
        sxwDrawLine(vecx, vecy, input[1].i, input[2].i);
        vecx = input[1].i;
        vecy = input[2].i;
      }
    } else if (input[0].uc[1] == 0xfe) {
      /*common endpoint vector*/
      SlideRecRead(&input[1], 1, in);

      sxwDrawLine(lastx, lasty, (input[0].c[0] + lastx),
                  (input[1].c[0] + lasty));
      lastx += input[0].c[0];
      lasty += input[1].c[0];
    } else if (input[0].uc[1] == 0xff) {
      /*change color*/
      sxColorSelect(AcadCol2Vga(input[0].c[0]), 0);
    } else
      printf("error\n");
  }
}
