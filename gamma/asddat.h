/***************************************************************************
 *
 *	  ASD Interpreter Header/Structure File
 *
 *	  Copyright (c) 1991, Chris Fanning.  All rights reserved.
 *	  This file is not for public consumption!
 *
 *	  (hope the public isn't hungry, eh?)
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

  /*ASDargs amount changed to 512 & argidx, argsleft changed to ints*/

  unsigned char ASDargs[512],   /* arguments for functions */
      ASDsafe;                  /* safe to take control away */
  int ASDargidx,                /* index into args */
      ASDargsleft;              /* still waiting for # args */
  unsigned char ASDrepCbuff,    /* character that's being repeated */
      ASDrepNbuff;              /* number of repetitions of above */
  struct _ASD_COMP *SavedState; /* saved state pointer */
  int ASDgmode;                 /*current graphics mode*/
} ASD_COMP;

#define NOTSAFE 0
