typedef unsigned char byte;
void InitScreen(void);
void ReadRecord(long recnum);
void DisplayRecord(void);
void DisplayGame(struct gdt *gm, int x);
void Copy1Field(struct gdt *source, struct gdt *dest, byte secondf);
void SortGame(struct gdt *info);
void Swap(struct gdt *info, int i, int j);
void ScanSafe(struct gdt *info);
void MakeSafe(char *info);
void SpaceOut(char *info);
int gameview(char *file);

char Modes[10][20] = {"  ", "PUBLIC", "LEAGUE", "FREE FOR ALL", "  "};

#pragma pack(1)
struct c_time {
  byte minute;
  byte hour;
  byte hsecond;
  byte second;
};

struct c_date {
  int year;
  byte day;
  byte month;
};

struct p_type {
  char passport[10];
  char name[10];
  int score;
};

struct gdt {
  char dummy[3];
  struct c_time time;
  struct c_date date;
  byte number;
  byte field;
  byte con_field;
  byte mode1;
  byte mode2;
  char redtm1[10];
  char grntm1[10];
  char redtm2[10];
  char grntm2[10];
  struct p_type player[40];
};
#pragma pack()
