#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamma.h"

#define readname(x)                                                    \
  ;                                                                    \
  {                                                                    \
    cgets(tempstring);                                                 \
    for (k = 2; k <= tempstring[1] + 1; ++k) x[k - 2] = tempstring[k]; \
    for (k = tempstring[1] + 2; k <= 9; ++k) x[k] = 0;                 \
  }

void setupmenu(void) {
  char tempstring[13];
  int i;
  char sel;
  char k, l;
  int plnum;
  FILE *fp;
  int exitit = 0;
  tempstring[0] = 11;
  do {
    printf("Please Select:\n");
    printf(" 1.View Current           2.ET1                    3.ET2\n");
    printf(
        " 4.Progress Computer      5.Data Computer          6.Effects "
        "Computer\n");
    printf(" 7.Phaser Stations        8.CD Track Selection     9.Field Type\n");
    printf(
        " A.Number of Players      B.Inbetween Screen Type  C.Beginning "
        "Scores\n");
    printf(
        " D.Handicapp Mode         E.Default Team Names     F.Game Length\n");
    printf(
        " G.Video Mode             H.Message File Path      I.Pod Performance "
        "Path\n");
    printf(
        " J.Database Path          K.Log File Path          Z.End "
        "Configuration\n");
    switch (toupper(getch())) {
      case '1':
        printf("Current Configuration:\n");
        printf("ET1:");
        switch (newconfig.et1) {
          case RED:
            printf("RED  ");
            break;
          case GREEN:
            printf("GREEN");
            break;
          case BOTH:
            printf("BOTH ");
        }
        printf("         ET2:");
        switch (newconfig.et2) {
          case RED:
            printf("RED  ");
            break;
          case GREEN:
            printf("GREEN");
            break;
          case BOTH:
            printf("BOTH ");
        }
        printf("\n");
        printf("Progress Computer:");
        switch (newconfig.pc) {
          case TRUE:
            printf("ON ");
            break;
          case FALSE:
            printf("OFF");
        }
        printf("     Data Computer:");
        switch (newconfig.dc) {
          case TRUE:
            printf("ON ");
            break;
          case FALSE:
            printf("OFF");
        }
        printf("\nEffects Computer:");
        switch (newconfig.ec) {
          case TRUE:
            printf("ON ");
            break;
          case FALSE:
            printf("OFF");
        }
        printf("     Phaser Stations:");
        switch (newconfig.ps) {
          case TRUE:
            printf("ON ");
            break;
          case FALSE:
            printf("OFF");
        }
        printf("\nCD Selection:");
        switch (newconfig.cdsel) {
          case RANDOM:
            printf("RANDOM ");
            break;
          case ORDERED:
            printf("ORDERED");
        }
        printf("      Inbetween screen:");
        switch (newconfig.tween) {
          case TEXT:
            printf("TEXT   ");
            break;
          case GRAPHIC:
            printf("GRAPHIC");
        }
        printf("\nBeginning Scores");
        printf("\nPublic:%d       League:%d      Free For All:%d",
               newconfig.pub_begin, newconfig.lea_begin, newconfig.ffa_begin);
        printf("\n\nHandicapping");
        printf("\nPublic:");
        switch (newconfig.pub_hcp) {
          case TRUE:
            printf("ON ");
            break;
          case FALSE:
            printf("OFF");
        }
        printf("    League:");
        switch (newconfig.lea_hcp) {
          case TRUE:
            printf("ON ");
            break;
          case FALSE:
            printf("OFF");
        }
        printf("    Free For All:");
        switch (newconfig.ffa_hcp) {
          case TRUE:
            printf("ON ");
            break;
          case FALSE:
            printf("OFF");
        }
        printf("\n\nTeam Names\n");
        printf("ALPHA RED:");
        for (i = 0; i < 10; ++i) putch(newconfig.alpha_red[i]);
        printf("     ALPHA GRN:");
        for (i = 0; i < 10; ++i) putch(newconfig.alpha_grn[i]);
        printf("\nOMEGA RED:");
        for (i = 0; i < 10; ++i) putch(newconfig.omega_red[i]);
        printf("     OMEGA GRN:");
        for (i = 0; i < 10; ++i) putch(newconfig.omega_grn[i]);
        printf("\nBETA  RED:");
        for (i = 0; i < 10; ++i) putch(newconfig.beta_red[i]);
        printf("\nBETA  GRN:");
        for (i = 0; i < 10; ++i) putch(newconfig.beta_grn[i]);
        printf("\nGame length:%d", newconfig.length);
        printf("\nPress any key to continue...");
        do {
        } while (getch() != 32);
        break;
      case '2': /*ET1*/
        printf("Config for ET1\n");
        printf("Options:\n");
        printf("0. OFF\n1. RED\n2. GREEN\n3.BOTH\n");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '4'));
        switch (k) {
          case '0':
            printf("ET1:OFF");
            newconfig.et1 = 0;
            break;
          case '1':
            printf("ET1:RED/ALPHA");
            newconfig.et1 = RED;
            break;
          case '2':
            printf("ET1:GREEN/OMEGA");
            newconfig.et1 = GREEN;
            break;
          case '3':
            printf("ET1:BOTH/BETA");
            newconfig.et1 = BOTH;
        }
        printf("\n");
        break;
      case '3': /*ET2*/
        printf("Config for ET2\n");
        printf("Options:\n");
        printf("0. OFF\n1. RED\n2. GREEN\n3.BOTH\n");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '4'));
        switch (k) {
          case '0':
            printf("ET2:OFF");
            newconfig.et2 = 0;
            break;
          case '1':
            printf("ET2:RED/ALPHA");
            newconfig.et2 = RED;
            break;
          case '2':
            printf("ET2:GREEN/OMEGA");
            newconfig.et2 = GREEN;
            break;
          case '3':
            printf("ET2:BOTH/BETA");
            newconfig.et2 = BOTH;
        }
        printf("\n");
        break;
      case '4': /*Progress Computer*/
        printf("Config for Progress Computer\n");
        printf("Options:\n");
        printf("0. OFF\n1. ON\n");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '1'));
        switch (k) {
          case '0':
            printf("Progress Computer:OFF");
            newconfig.pc = FALSE;
            break;
          case '1':
            printf("Progress Computer:ON");
            newconfig.pc = TRUE;
        }
        printf("\n");
        break;
      case '5': /*Data Computer*/
        printf("Config for Data Computer\n");
        printf("Options:\n");
        printf("0. OFF\n1. ON\n");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '1'));
        switch (k) {
          case '0':
            printf("Data Computer:OFF");
            newconfig.dc = FALSE;
            break;
          case '1':
            printf("Data Computer:ON");
            newconfig.dc = TRUE;
        }
        printf("\n");
        break;
      case '6': /*Effects Computer*/
        printf("Config for Effects Computer\n");
        printf("Options:\n");
        printf("0. OFF\n1. ON\n");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '1'));
        switch (k) {
          case '0':
            printf("Effects Computer:OFF");
            newconfig.ec = FALSE;
            break;
          case '1':
            printf("Effects Computer:ON");
            newconfig.ec = TRUE;
        }
        printf("\n");
        break;
      case '7': /*Phaser Stations*/
        printf("Config for Phaser Stations\n");
        printf("Options:\n");
        printf("0. OFF\n1. ON\n");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '1'));
        switch (k) {
          case '0':
            printf("Phaser Stations:OFF");
            newconfig.ps = FALSE;
            break;
          case '1':
            printf("Phaser Stations:ON");
            newconfig.ps = TRUE;
        }
        printf("\n");
        break;
      case '8': /*CD tracks*/
        printf("Config for CD Tracks\n");
        printf("Options:\n");
        printf("0. RANDOM\n1. ORDERED\n");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '1'));
        switch (k) {
          case '0':
            printf("CD Tracks:RANDOM");
            newconfig.cdsel = RANDOM;
            break;
          case '1':
            printf("CD Tracks:ORDERED");
            newconfig.cdsel = ORDERED;
        }
        printf("\n");
        break;
      case '9': /*Field Type*/
        printf("Config for Field Type\n");
        printf("Options:\n");
        printf("0. SINGLE\n1. DUAL\n");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '1'));
        switch (k) {
          case '0':
            printf("Field Type:BETA single field");
            newconfig.field = 1;
            break;
          case '1':
            printf("Field Type:ALPHA/OMEGA dual field");
            newconfig.field = 2;
        }
        printf("\n");
        break;
      case 'A': /*Number of Players*/
        printf("Config for Number of Players\n");
        printf("Options:\n");
        printf("Enter # of players (8-15) per team:");
        do {
          scanf("%d", &plnum);
          if ((plnum < 16) & (plnum > 7))
            printf("Number of Players:%d", plnum);
          else
            printf("Number out of range");
        } while ((plnum > 15) & (plnum < 8));
        newconfig.players = plnum;
        break;
      case 'B': /*'Tween screens*/
        printf("Config for Inbetween Screens\n");
        printf("Options:\n");
        printf("0. TEXT\n1. GRAPHIC\n");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '1'));
        switch (k) {
          case '0':
            printf("Inbetween Screens:TEXT");
            newconfig.tween = TEXT;
            break;
          case '1':
            printf("Inbetween Screens:GRAPHIC");
            newconfig.tween = GRAPHIC;
        }
        printf("\n");
        break;
      case 'C': /*Beginning Scores*/
        printf("Config for Beginning Scores\n");
        printf("Public:");
        scanf("%d", &newconfig.pub_begin);
        printf("League:");
        scanf("%d", &newconfig.lea_begin);
        printf("Free For All:");
        scanf("%d", &newconfig.ffa_begin);
        printf("\n");
        break;
      case 'D': /*Handicap Mode*/
        printf("Config for Public Handicapping\n");
        printf("Options:\n");
        printf("0. OFF\n1. ON\n");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '1'));
        switch (k) {
          case '0':
            printf("Public Handicapp:OFF");
            newconfig.pub_hcp = FALSE;
            break;
          case '1':
            printf("Public Handicapp:ON");
            newconfig.pub_hcp = TRUE;
        }
        printf("\n");
        printf("Config for League Handicapping\n");
        printf("Options:\n");
        printf("0. OFF\n1. ON\n");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '1'));
        switch (k) {
          case '0':
            printf("League Handicapp:OFF");
            newconfig.lea_hcp = FALSE;
            break;
          case '1':
            printf("League Handicapp:ON");
            newconfig.lea_hcp = TRUE;
        }
        printf("\n");
        printf("Config for Free For All Handicapp\n");
        printf("Options:\n");
        printf("0. OFF\n1. ON\n");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '1'));
        switch (k) {
          case '0':
            printf("Free For All Handicapp:OFF");
            newconfig.ffa_hcp = FALSE;
            break;
          case '1':
            printf("Free For All Handicapp:ON");
            newconfig.ffa_hcp = TRUE;
        }
        printf("\n");
        break;
      case 'E': /*Default Team Names*/
        printf("Config for Default Team Names\n");
        printf("Enter default name (for PUBLIC & FREE FOR ALL) at prompt");
        printf("\nField 1 RED:");
        readname(newconfig.alpha_red);
        printf("\nField 1 GRN:");
        readname(newconfig.alpha_grn);
        printf("\nField 2 RED:");
        readname(newconfig.omega_red);
        printf("\nField 2 GRN:");
        readname(newconfig.omega_grn);
        printf("\nSingle RED:");
        readname(newconfig.beta_red);
        printf("\nSingle GRN:");
        readname(newconfig.beta_grn);
        break;
      case 'F':
        printf("Enter new game length in seconds:");
        scanf("%d", &newconfig.length);
        break;
      case 'G':
        printf("Enter video mode:\n0.  CGA  \n1.  TEST");
        k = 0;
        do
          k = getch();
        while ((k < '0') & (k > '1'));
        switch (k) {
          case '0':
            printf("Video Mode:CGA");
            newconfig.vidmode = 0;
            break;
          case '1':
            printf("Video Mode:VGA");
            newconfig.vidmode = 1;
        }
        break;
      case 'H':
        printf("Enter current path for Message files\n");
        scanf("%s", newconfig.mess_path);
        break;
      case 'I':
        printf("Enter current path for Pod Performance files\n");
        scanf("%s", newconfig.pod_path);
        break;
      case 'J':
        printf("Enter current path for Database files\n");
        scanf("%s", newconfig.data_path);
        break;
      case 'K':
        printf("Enter current path for System Log file\n");
        scanf("%s", newconfig.log_path);
        break;

      case 'Z':
        if ((newconfig.et1 == newconfig.et2) |
            ((newconfig.field == 2) && (newconfig.et1 == BOTH))) {
          printf("\n\007\007\007ET CONFIGURATION INCORRECT...TRY AGAIN");
          exitit = 0;
        } else
          exitit = 1;
    }
  } while (exitit == 0);
  printf("Do you wish to save this configuration to disk ?");
  sel = ' ';
  do
    sel = toupper(getch());
  while ((sel != 'Y') & (sel != 'N'));
  if (sel == 'Y')
    if ((fp = fopen("system.ini", "w")) != NULL) {
      fwrite(&newconfig, sizeof(newconfig), 1, fp);
      fclose(fp);
    }
}
