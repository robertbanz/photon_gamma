/***************************************************************************
 *																									*
 *	  ASD Interpreter Header/Structure File
 **
 *																									*
 *	  Copyright (c) 1991, Chris Fanning.  All rights reserved.
 ** This file is not for public consumption!
 **
 *																									*
 ***************************************************************************/

typedef struct _ASD_COMP {
  int (*recv)(void),             /* stdin function for menus etc. */
      (*rxstat)(void);           /* stdin status function */
  void (*output)(unsigned char), /* standard output for ASD */
      (*rawout)(unsigned char),  /* raw output (serial etc) */
      (*respnd)(unsigned char),  /* response fn (avoid feedback) */
      (*flow)(unsigned char),    /* flow control function */
      (*ASDcurfn)(void),         /* current function waiting on */
      (*ASDput)(unsigned char);  /* current output function */
  unsigned char ASDargs[130],    /* arguments for functions */
      ASDsafe,                   /* safe to take control away */
      ASDargidx,                 /* index into args */
      ASDargsleft,               /* still waiting for # args */
      ASDrepCbuff,               /* character that's being repeated */
      ASDrepNbuff;               /* number of repetitions of above */
  struct _ASD_COMP *SavedState;  /* saved state pointer */
} ASD_COMP;

#define NOTSAFE 0
