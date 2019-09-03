#include <graph.h>
#include <stdio.h>

/*Microsoft C font operations routines*/
struct fctl_t {
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

void fn_init(struct fctl_t control) {
  /* Read header info from .FON files in current or given directory. */
  if (_registerfonts("*.FON") <= 0) {
    if (_registerfonts("C:\\MSC\\SOURCE\\SAMPLES\\*.FON") <= 0) {
      _outtext("Error: can't register fonts");
      exit(1);
    }
  }
  fontcontrol = control;
  fontcontrol.maxx = 640;
  fontcontrol.maxy = 480;
  _setfont(control.fonts[1]);
}

void fn_setfont(int number) {
  _setfont(fontcontrol.fonts[number]);
  _getfontinfo(&fontcontrol.fi);
}

void fn_newline(void) {
  fontcontrol.y += fontcontrol.fi.pixheight + 4;
  fontcontrol.x = 0;
}

void fn_clear(void) {
  _clearscreen(_GCLEARSCREEN);
  fontcontrol.y = 0;
  fontcontrol.x = 0;
}

void fn_color(int foreground, int backround) {
  fontcontrol.color = foreground;
  _setcolor(fontcontrol.color);
}

void fn_printtext(int x, int y, char *text) {
  struct xycoord temp;
  int oldcolor;

  temp = _moveto(fontcontrol.x, fontcontrol.y);

  fontcontrol.x = temp.xcoord;
  fontcontrol.y = temp.ycoord;

  fontcontrol.x = fn_position(fontcontrol.x, x, fontcontrol.fi.pixwidth, text);
  fontcontrol.y =
      fn_position(fontcontrol.y, y, fontcontrol.fi.pixheight + 4, text);

  temp = _moveto(fontcontrol.x, fontcontrol.y);

  oldcolor = _setcolor(fontcontrol.color);

  _outgtext((unsigned char _far *)text);

  _setcolor(oldcolor);
  /*_moveto(temp.xcoord, temp.ycoord);*/
}

int fn_position(int oldpos, int now, int size, char *text) {
#define INCR 1024
#define MINU 2048
#define ABS 4096
#define CEN 8192
#define FBOT 16384

  if (now & FBOT) {
    if (now & ABS)
      return (fontcontrol.maxy - (now & 1023));
    else
      return (fontcontrol.maxy - (size * (now & 1023)));
  } else if (now & INCR) { /*it's an incremental move!*/
    if (now & MINU)
      if (now & ABS)
        return (oldpos - (now & 1023));
      else
        return (oldpos - ((now & 1023) * size));
    else if (now & ABS)
      return (oldpos + (now & 1023));
    else
      return (oldpos + ((now & 1023) * size));
  } else if (now == CEN) /*ok let's center it!*/
    return ((fontcontrol.maxx - _getgtextextent((unsigned char _far *)text)) /
            2);
  else { /*it's not an incremental move*/
    if (now & ABS)
      return (now & 1023);
    else
      return (size * (now & 1023));
  }
}
