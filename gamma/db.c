#include "db.h"

#ifdef SYSTEM_PROGRAM
db_rec *db_create_from_memory();
#endif

db_rec *db_create_from_oldrec(old_db_game *ogr) {
  int players = 0;
  db_rec *db;
  int i;

  for (i = 0; i < 40; ++i)
    if (ogr->pl.passport[0] != 32) ++players;

    if ((db = malloc(sizeof(db_rec) + sizeof (db_player) * (players - 1)) == NULL)
	exit(1);
    x



void db_player_id_from_passport(char *, player_id *);
