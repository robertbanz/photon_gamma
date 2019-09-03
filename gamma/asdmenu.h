struct mPalette {
  struct mPalette *previous; /* previous palette */
  unsigned char borderstyle; /* border style to use */

  unsigned char menuborder; /* border color of menu */
  unsigned char menucolor;  /* forground color of menu */
  unsigned char menuselect; /* selected menu item */
  unsigned char menudim;    /* dimmed menu color */

  unsigned char editborder; /* edit box border color */
  unsigned char editcolor;  /* edit box text color */

  unsigned char yesnoborder; /* yesno menu border */
  unsigned char yesnocolor;  /* yesno menu color */
  unsigned char yesnoselect; /* yesno select color */

  unsigned char alertborder; /* alert box border */
  unsigned char alertcolor;  /* alert box color */
};

#define mOK 0        /* all went well */
#define mNOMEM -1    /* insufficient memory for operation */
#define mNOMARKER -2 /* no marker exists */
#define mNOMENU -3   /* lastMarker exists, with no menu on screen */
#define mLOOP -4
#define mNOITEMS -5 /* sent if current list is empty */

/*	These values are returned by mPopUpMenu, positive values correspond to
        the number of the item selected... (1 to num_items) */
#define pESC -1    /* sent when user hits ESC at menu */
#define pHELP -3   /* sent if user presses CTRL-H */
#define pSELECT -4 /* sent if user presses return */

int mAppendToMenu(char *mtext, int retval, unsigned char hkey);
int mPushMarker(void);
int mPopMarker(void);
int mPopUpMenu(unsigned char xpos, unsigned char ypos, int (*menuaction)(int),
               int itemidx, unsigned char maxshow);
int mDestroyMenu(void);
int mMaskMenu(struct mItemMarker *dimmenu);
void mUnMaskMenu(struct mItemMarker *dimmenu);
int mAlert(char *message, unsigned char xpos, unsigned char ypos);
void mCenterText(unsigned char xwhere, unsigned char ywhere,
                 unsigned char xlength, char *message);
int mEString(char *buffer, unsigned char length, unsigned char resume);
void mResetMenu(void);
int mEditString(char *buffer, char *prompt, unsigned char length,
                unsigned char xpos, unsigned char ypos);
int mYesNo(char *prompt, unsigned char xpos, unsigned char ypos, int mdefault);
int NOESC(int select);
void mPushPalette(struct mPalette *newpal);
void mPopPalette(void);
int mSaveState(void);
int mRestoreState(void);
