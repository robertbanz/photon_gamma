/***********
 initper.c
************/

#include <c:\tc\gamma.h>
#include <stdlib.h>

#include <dos.h>
#include <stdio.h>

struct podstruct data;

main() {
  FILE *fp;
  data.podid = 99;
  data.id = 00;
  fp = fopen("b:\pod.red", "w");
  fwrite(&data, sizeof(data), 1, fp);
  fclose(fp);
  fp = fopen("b:\pod.grn", "w");
  fwrite(&data, sizeof(data), 1, fp);
  fclose(fp);
}
