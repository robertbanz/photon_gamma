/**************
 AVGBAT .C
***************/

#define __TOPLEV

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include "gamma.h"

struct podstructflt {
  double id, base, rxmiss, rxbad, txmiss, txbad, valcode, hitackn, hitsent;
  byte podid;
  byte missinrow;
  byte resetflag;
};

/* Pod Performance AVGBAT program */
void Perform(char *);

FILE *printpath;
main() {
  char where;
  char path[30];
  char redpath[40], grnpath[40];
  SetupPodId();
  system("cls");
  printf("Pod Performace Avgerage Program\n");
  printf("\n");
  printf("-------------------------------\n");
  printf("Please specify path to read from:");
  scanf("%s", path);
  strcpy(redpath, path);
  strcpy(grnpath, path);
  strcat(redpath, "pod.red");
  strcat(grnpath, "pod.grn");
  printf("Do you wish to print to the [P]rinter or [F]ile\n");
  while (!(kbhit()))
    ;
  if (toupper(getch()) == 'P') {
    where = 'P';
    printpath = stdprn;
  } else {
    where = 'F';
    printpath = fopen("avgbat.dat", "w");
  }
  Perform(redpath);
  Perform(grnpath);
  if (where == 'F') fclose(printpath);
}

void Perform(char *path) {
  FILE *fp;
  int i;
  int num[45];
  int gameno;
  struct podstruct data;
  struct podstructflt tot[45];
  for (i = 0; i < 45; ++i) {
    tot[i].id = 0;
    tot[i].base = 0;
    tot[i].rxmiss = 0;
    tot[i].rxbad = 0;
    tot[i].txmiss = 0;
    tot[i].txbad = 0;
    tot[i].valcode = 0;
    tot[i].hitackn = 0;
    tot[i].hitsent = 0;
    tot[i].podid = 0;
    num[i] = 0;
  }
  printf("Reading from:%s\n", path);
  printf("-------------------------------\n");
  if ((fp = fopen(path, "rb")) == NULL)
    printf("Error\n");
  else {
    fprintf(printpath, "\n%s             POD EVALUATION\n", path);
    fprintf(printpath, "-----------------------------\n");
    fprintf(printpath, "            RESETS           \n");
    fprintf(printpath, "-----------------------------\n");
    fread(&data, sizeof(struct podstruct), 1, fp);
    printf("Reading first header\n");
    gameno = data.id;
    while (fread(&data, sizeof(struct podstruct), 1, fp) != 0) {
      if (data.podid == 99)
        gameno = data.id;
      else {
        i = data.podid;
        if (data.resetflag != '*') {
          tot[i].id += data.id;
          tot[i].base += data.base;
          tot[i].rxmiss += data.rxmiss;
          tot[i].rxbad += data.rxbad;
          tot[i].txmiss += data.txmiss;
          tot[i].txbad += data.txbad;
          tot[i].valcode += data.valcode;
          tot[i].hitackn += data.hitackn;
          tot[i].hitsent += data.hitsent;
          ++tot[i].podid;
        } else {
          ++num[i];
          if (i != 0)
            fprintf(printpath, "GAME:%d       POD:%d\n", (gameno - 1), i);
        }
      }
    }
  }
  fprintf(printpath, "\f");
  fprintf(printpath, "%s             POD EVALUATION\n", path);
  fprintf(printpath, "-----------------------------\n");
  fprintf(printpath, "       RF PERFORMANCE        \n");
  fprintf(printpath, "-----------------------------\n");
  fprintf(printpath, "\n\n\n\n");
  fprintf(printpath, "                               POD TX/      CRAD TX/\n");
  fprintf(printpath, "           AVG GAME ACTION     CRAD RX      POD RX\n");
  fprintf(printpath, "           ---------------     ---------    ---------\n");
  fprintf(
      printpath,
      "POD    #    ID  BASE   VAL     BAD   NO     BAD   NO     ---HITS---\n");
  fprintf(printpath,
          " #    GMS       CODE  CODE     CODE  COM    CODE  COM    SENT  ACKN "
          "  RESET\n");
  fprintf(printpath,
          "--------------------------------------------------------------------"
          "-------\n\n\n");
  for (i = 1; i <= 44; ++i)
    if (tot[i].podid > 0) {
      tot[i].id = tot[i].id / tot[i].podid;
      tot[i].base = tot[i].base / tot[i].podid;
      tot[i].rxmiss = tot[i].rxmiss / tot[i].podid;
      tot[i].rxbad = tot[i].rxbad / tot[i].podid;
      tot[i].txmiss = tot[i].txmiss / tot[i].podid;
      tot[i].txbad = tot[i].txbad / tot[i].podid;
      tot[i].valcode = tot[i].valcode / tot[i].podid;
      tot[i].hitackn = tot[i].hitackn / tot[i].podid;
      tot[i].hitsent = tot[i].hitsent / tot[i].podid;

      fprintf(printpath,
              "%3d   %3d  %3d   %3d   %3d      %3d  %3d     %3d  %3d     %3d   "
              "%3d    %3d\n",
              i, (int)(tot[i].podid + num[i]), (int)tot[i].id, (int)tot[i].base,
              (int)tot[i].valcode, (int)tot[i].txbad, (int)tot[i].txmiss,
              (int)tot[i].rxbad, (int)tot[i].rxmiss, (int)tot[i].hitsent,
              (int)tot[i].hitackn, (int)num[i]);
    }
  fprintf(printpath, "\f");
}
