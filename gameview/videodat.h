/***************************************************************************
 *																									*
 *	  Text Video Routine File Header
 **
 *																									*
 *	  Copyright (c) 1991, Chris Fanning.  All rights reserved.
 ** This file is not for public consumption!
 **
 *																									*
 ***************************************************************************/

#define NUM_IM 32
#define CNSAV 16

typedef struct {
  unsigned short idx, /* index into page memory */
      crsx,           /* current X position */
      crsy;           /* current Y position */
} cposition;

typedef struct {
  unsigned short idx, /* index into page memory */
      crsx,           /* saved X position */
      crsy,           /* saved Y position */
      ctype;          /* saved cursor type */
} csavedpos;

typedef struct {
  unsigned short _far *pagemem; /* page memory pointer */
  unsigned short storedidx,     /* current index into stored */
      curattr,                  /* current attribute */
      crsupdat,                 /* update cursor? 0=no other=yes */
      curtype;                  /* current type of cursor */
  cposition writepos;           /* current write position */
  cposition crspos;             /* visual cursor position */
  csavedpos stored[CNSAV];      /* stored cursor positions */
} vpagestuff;

typedef struct {
  unsigned short idata, /* index into imagemem */
      iwidth,           /* width of image */
      iheight,          /* image height */
      iused;            /* is the image REALLY used? */
} imagespecs;

struct vcontrol {
  vpagestuff pages;              /* page structure pointer */
  unsigned short numidx,         /* number of idx's on screen */
      CRTC,                      /* CRTCs I/O address */
      imemleft,                  /* amount of image memory left */
      height,                    /* height of screen */
      swidth,                    /* width of REAL screen */
      curpage,                   /* current page # we're writing to */
      textmode,                  /* text mode for shelling... */
      cardref;                   /* video card reference used in arrays */
  unsigned short _far *imagemem, /* stored image memory */
      _far *linemem,             /* linebuffer storage */
      _far *vrealmem;            /* real memory skirts around rediretion */
  imagespecs images[NUM_IM];     /* image data */
};

/* globals available to everyone */
struct VIDhandle {
  unsigned char Video0Type, Display0Type, Video1Type, Display1Type;
};
