#include <conio.h>
#include <dos.h>
#include "asd.h"
#include "asdmenu.h"
#include "video.h"
/*Database update program (c) 1992 Robert Banz, all rights reserved*/

struct tme {
  byte minute;
  byte hour;
  byte hsecond;
  byte second;
};

struct dte {
  int year;
  byte day;
  byte month;
};

struct playerst {
  char passport[10];
  char name[10];
  int score;
};
struct gameview {
  char dummy[3];
  struct tme tm;
  struct dte dt;
  byte number, field, config, mode1, mode2;
  char redtm1[10], grntm1[10], redtm2[10], grntm2[10];
  struct playerst pl[40];
};
FILE *src, *dest;

main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Bad or missing command line arguements\n");
    exit(-1);
  }
    if ((src = fopen(argv[1],"r") == NULL) {
    printf("Error opening source file\n");
    exit(-1);
	}
    if ((dest = fopen(argv[2],"ab") == NULL) {
    printf("Error opening dest. file\n");
    exit(-1);
	}
    ASDInit(
