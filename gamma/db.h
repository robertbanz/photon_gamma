#define DB_CURRENT_REV 1
typedef unsigned char byte;
typedef unsigned long game_id;

struct tme {
  byte minute;
  byte hour;
  byte hsecond;
  byte second;
};

struct dte {
  int year;
  byte day;
  byte month;
};

typedef struct old_db_playerst {
  char passport[10];
  char name[10];
  int score;
} old_db_player;

typedef struct old_db_gameviewst {
  char dummy[3];
  struct tme time;
  struct dte date;
  byte number, field, config, mode1, mode2;
  char redtm1[10], grntm1[10], redtm2[10], grntm2[10];
  old_db_player pl[40];
} old_db_game;

typedef struct player_id_struct {
  unsigned int store_num;
  unsigned long db_id;
} player_id;

typedef struct db_player_struct {
  char cname[15];
  player_id id;
  int score;
  int hits, gethits, oops;
  unsigned char get_base;
  game_id next_game;
  game_id last_game;
} db_player;

typedef struct db_ginfo_struct {
  unsigned int year;
  unsigned char month, day, hour, minute, second;
  int max_length;
  int acutal_length;
  int beginning_score;
  int base_score;
  unsigned char hcapping;
} db_ginfo;

typedef struct db_rec_struct {
  unsigned char db_rev;
  long id;
  db_ginfo info;
  unsigned char game_type;
  char team_name[2][15];
  unsigned char flag;
  unsigned char team_plrs[2];
  db_player pl;
} db_rec;

db_rec *db_create_from_memory();
db_rec *db_create_from_oldrec(old_db_game *);
void db_player_id_from_passport(char *, player_id *);
