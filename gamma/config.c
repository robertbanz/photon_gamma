/***********
 Config.c
 ***********/

/*******PHOTON GAMMA SYSTEM CONFIGURATION PROGRAM*******/

/* Copyright (c) 1991-1992 Robert Banz, All Rights Reserved */

#include <stdio.h>
#include "asd.h"
#include "asdmenu.h"
#include "gamma.h"
#include "video.h"
#include "videodat.h"

extern struct vcontrol vbase;

struct config newconfig;
FILE *fp;

main() {
  FILE *fp;
  char answer;
  ASDInit(FINDADAPTER);
  if (vbase.cardref == 0) MonoPalette();
  if ((fp = fopen("system.ini", "r")) == NULL) {
    printf("Initialization file not available...\n");
    printf("Please specify your preferences:\n");
    setupmenu();
  } else {
    fread(&newconfig, sizeof(struct config), 1, fp);
    fclose(fp);
    setupmenu();
  }
}
