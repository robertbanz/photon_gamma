#include <graph.h>

extern struct fctl_t {
  char *fonts[10];
  int x;
  int y;
  struct _fontinfo fi;
  int color;
  int maxx;
  int maxy;
} fontcontrol;

void fn_init(struct fctl_t);
void fn_setfont(int number);
void fn_newline(void);
void fn_clear(void);
void fn_color(int foreground, int backround);
void fn_printtext(int x, int y, char *text);
int fn_charheight(int number);

#define INCR 1024
#define MINU 2048
#define ABS 4096
#define CEN 8192
#define FBOT 16384
