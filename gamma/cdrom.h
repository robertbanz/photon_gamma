/* my "generic" system and compiler dependant typedefs, stick in sep. file */
typedef signed char I8;
typedef unsigned char U8;
typedef signed short int I16;
typedef unsigned short int U16;
typedef signed long int I32;
typedef unsigned long int U32;

/* DEV_REQ_HEADER "status" bits */
#define STATUS_ERROR 0x8000
#define STATUS_BUSY 0x0200
#define STATUS_DONE 0x0100
#define STATUS_CODE(x) ((x)&0x00ff)

#define CD_MODE_COOKED 0
#define CD_MODE_RAW 1

#define CD_SECTSIZE_COOKED 2048
#define CD_SECTSIZE_RAW 2352
#define CD_NUM_SAMP_SECTOR 588

#define CD_ADDR_HSG 0
#define CD_ADDR_RED_BOOK 1

#define CD_STAT_DOOR_OPEN 0x0001
#define CD_STAT_DOOR_UNLOCKED 0x0002
#define CD_STAT_RAW_MODE 0x0004
#define CD_STAT_WRITEABLE 0x0008
#define CD_STAT_MULTIMEDIA 0x0010
#define CD_STAT_INTERLEAVE 0x0020
#define CD_STAT_PREFETCHES 0x0080
#define CD_STAT_AUDIO_CHANNELS 0x0100
#define CD_STAT_RED_BOOK 0x0200
#define CD_STAT_NO_DISC 0x0800
#define CD_STAT_SUBCHANNELS 0x1000

#define CD_DOOR_UNLOCK 0
#define CD_DOOR_LOCK 1

#define RED_TO_HSG(x) (x.frame + ((x.sec + (x.min * 60)) * 75))
#define CDROMReadRaw(a, b, c, d) CDROMRead(a, b, c, CD_MODE_RAW, d)
#define CDROMReadCooked(a, b, c, d) CDROMRead(a, b, c, CD_MODE_COOKED, d)

#pragma pack(1)
typedef struct _CD_DA_CHAN {
  U8 LSB,  /* LSB portion of 16-bit DA sample */
      MSB; /* MSB portion of 16-bit DA sample */
} CD_DA_CHAN;

typedef union _CD_DA_UNION {
  CD_DA_CHAN b; /* byte sized DA information */
  U16 x;        /* complete word sized DA */
} CD_DA_UNION;

typedef struct _CD_DA_SAMPLE {
  CD_DA_UNION left;  /* left channel */
  CD_DA_UNION right; /* right channel */
} CD_DA_SAMPLE;

typedef struct _CD_DA_SECTOR {
  CD_DA_SAMPLE samp[CD_NUM_SAMP_SECTOR];
} CD_DA_SECTOR;

typedef struct _CD_RED_BOOK {
  U8 frame, /* minutes */
      sec,  /* seconds */
      min,  /* frames */
      zero; /* always zero */
} CD_RED_BOOK;

typedef union _CD_ADDR {
  CD_RED_BOOK redBook; /* Red Book address */
  U32 block;           /* logical block number, HSG mode */
} CD_ADDR;

typedef struct _CD_HEAD_LOCATION {
  U8 mode;          /* addressing mode */
  CD_ADDR location; /* location of drive head */
} CD_HEAD_LOCATION;

typedef struct _CD_AUDIO_CHANNELS {
  U8 chan0,  /* input chan (0,1,2,3) on chan0 */
      vol0,  /* volume chan0, 0 -> 0xff */
      chan1, /* input chan (0,1,2,3) on chan1 */
      vol1,  /* volume chan1, 0 -> 0xff */
      chan2, /* input chan (0,1,2,3) on chan2 */
      vol2,  /* volume chan2, 0 -> 0xff */
      chan3, /* input chan (0,1,2,3) on chan3 */
      vol3;  /* volume chan3, 0 -> 0xff */
} CD_AUDIO_CHANNELS;

typedef struct _CD_DISC_INFO {
  U8 lowTrack,       /* lowest track # */
      highTrack;     /* highest track # */
  CD_RED_BOOK start; /* starting point of lead-out track */
} CD_DISC_INFO;

typedef struct _CD_TRACK_INFO {
  U8 track;          /* track we want info on */
  CD_RED_BOOK start; /* starting point of track */
  U8 control;        /* track control info */
                     /* control is of this format:
                             00x00000	- 2 audio channels without pre-emphasis
                             00x10000	- 2 audio channels with pre-emphasis
                             10x00000	- 4 audio channels without pre-emphasis
                             10x10000	- 4 audio channels with pre-emphasis
                             01x00000	- data track
                             01x10000	- reserved
                             11xx0000	- reserved
                             xx0x0000	- digital copy prohibited
                             xx1x0000	- digital copy permitted		*/
} CD_TRACK_INFO;

typedef struct _CD_Q_CHANNEL {
  U8 control, /* control & ADR byte */
      track,  /* track # (TNO) */
      index,  /* (point) or Index (X) */
      /* running time within a track: */
      min,   /* minutes (MIN) */
      sec,   /* seconds (SEC) */
      frame, /* frames (FRAME) */
      zero,  /* zero? (ZERO) */
      /* running time on the disc: */
      amin,   /* minutes (AMIN) or (PMIN) */
      asec,   /* seconds (ASEC) or (PSEC) */
      aframe; /* frames (AFRAME) or (PFRAME) */
} CD_Q_CHANNEL;

typedef struct _CD_AUDIO_STATUS {
  U16 status;    /* audio status bits:
                                                 0, audio paused
                                                 1-15, reserved */
  CD_ADDR start, /* starting location for last play */
      end;       /* ending location for last play */
} CD_AUDIO_STATUS;

typedef struct _CD_SECTOR_SIZE {
  U8 mode;  /* read mode interested in (raw/cooked)*/
  U16 size; /* sector size */
} CD_SECTOR_SIZE;

#pragma pack()
