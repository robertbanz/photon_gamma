/***************************************************************************
 *																									*
 *	  ASD Menuing System	Header/Structure File
 **
 *																									*
 *	  Copyright (c) 1991, Chris Fanning.  All rights reserved.
 ** This file is not for public consumption!
 **
 *																									*
 ***************************************************************************/

#define MEN_MAXWIDTH 80

struct mMenuItem {
  unsigned char text[MEN_MAXWIDTH]; /* menu entry text */
  unsigned char width;              /* width of menuitem */
  unsigned char hotkey;             /* shortcut key for item */
  int rval;                         /* return value of item */
  struct mMenuItem *next;           /* pointer to next item */
  struct mMenuItem *prev;           /* previous menu item */
};

struct mItemMarker {
  struct mMenuItem *first;      /* first item */
  struct mMenuItem *last;       /* append items HERE */
  struct mItemMarker *previous; /* previous marker */
  int mWhere;                   /* currently selected item number */
  int mTop;                     /* item at top of menu */
  int numItems;                 /* number of menu items */
  int maxVisible;               /* max number of items visible */
  unsigned char widest;         /* biggest width so far */
  unsigned char savedBuff;      /* buffer screen is saved to */
  unsigned char xWhere;         /* beginning x position of menu */
  unsigned char yWhere;         /* beginning y position of menu */
};
