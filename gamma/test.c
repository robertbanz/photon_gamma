#include "cdromf.h"
#include "gamma.h"
#include "phoproto.h"

main(int argc, char **argv) {
  CD_reset();
  CD_track(1);
  CD_play();
}
