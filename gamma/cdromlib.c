#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "cdrom.h"

/***** DEFINEs *************************************************************/
/* DEV_REQ_HEADER valid "commands" for CD-ROM */
/*#define DEV_INIT				0*/
#define DEV_IOCTL_INPUT 3
#define DEV_INPUT_FLUSH 7
#define DEV_IOCTL_OUTPUT 12
#define DEV_DEVICE_OPEN 13
#define DEV_DEVICE_CLOSE 14
#define DEV_READ_LONG 128
#define DEV_READ_LONG_PRE 130
#define DEV_SEEK 131
#define DEV_AUDIO_PLAY 132
#define DEV_AUDIO_STOP 133
#define DEV_AUDIO_RESUME 136

/* IOCTL Input codes for CD-ROM drivers */
#define IOCTL_IN_HEADER_ADDRESS 0
#define IOCTL_IN_HEAD_LOCATION 1
#define IOCTL_IN_ERROR_STATS 3
#define IOCTL_IN_AUDIO_CHANNELS 4
/*#define IOCTL_IN_DRIVE_BYTES		5*/
#define IOCTL_IN_DEVICE_STATUS 6
#define IOCTL_IN_SECTOR_SIZE 7
#define IOCTL_IN_VOLUME_SIZE 8
#define IOCTL_IN_MEDIA_CHANGED 9
#define IOCTL_IN_AUDIO_DISC 10
#define IOCTL_IN_AUDIO_TRACK 11
#define IOCTL_IN_AUDIO_Q_CHANNEL 12
/*#define IOCTL_IN_AUDIO_S_CHANNEL	13*/
#define IOCTL_IN_UPC_CODE 14
#define IOCTL_IN_AUDIO_STATUS 15

#define IOCTL_OUT_EJECT_DISC 0
#define IOCTL_OUT_DOOR_CONTROL 1
#define IOCTL_OUT_RESET 2
#define IOCTL_OUT_AUDIO_CONTROL 3
#define IOCTL_OUT_CLOSE_TRAY 5

/***** TYPEs ***************************************************************/
#pragma pack(1)
/* device header structure, used when travelling device list */
typedef struct _DEV_HEADER {
  struct _DEV_HEADER _far *next; /* pointer to next device */
  I16 attributes;                /* device capabilities */
  I16 strategyOffset;            /* strat routine offset */
  I16 interruptOffset;           /* int routine offset */
  I8 name[8];                    /* device name */
  I16 reserved;
  I8 driveLetter; /* drive letter of device */
  I8 numUnits;    /* ?? */
} DEV_HEADER;

/* device request header, used for all direct device calls */
typedef struct _DEV_REQ_HEADER {
  U8 length;      /* # of bytes in request header */
  U8 subUnit;     /* subunit code */
  U8 command;     /* command code for driver */
  U16 status;     /* return status */
  U8 reserved[8]; /* reserved */
} DEV_REQ_HEADER;

/* used to talk to the CD-ROM driver about it's capabilities, etc */
typedef struct _IOCTL_HEADER {
  DEV_REQ_HEADER devReq; /* device request header */
  U8 media;              /* media descriptor byte (0) */
  void _far *transAddr;  /* transfer address */
                         /*  always points to code */
  U16 numBytes;          /* # of bytes to transfer */
  U16 startSector;       /* starting sector number (0) */
  void _far *volume;     /* no idea, (0) */
  U8 code;               /* IOCTL code request */
} IOCTL_HEADER;

typedef struct _IOCTL_I_HEADER_ADDRESS {
  IOCTL_HEADER ioctl; /* IOCTL header */
  void _far *addr;    /* pointer to device header */
} IOCTL_I_HEADER_ADDRESS;

typedef struct _IOCTL_I_HEAD_LOCATION {
  IOCTL_HEADER ioctl;    /* IOCTL header */
  CD_HEAD_LOCATION info; /* head&mode info */
} IOCTL_I_HEAD_LOCATION;

/* doubles for INPUT and OUTPUT IOCTLs, same structure is used */
typedef struct _IOCTL_AUDIO_CHANNELS {
  IOCTL_HEADER ioctl;     /* IOCTL header */
  CD_AUDIO_CHANNELS info; /* audio channel info */
} IOCTL_AUDIO_CHANNELS;

/*typedef struct _IOCTL_I_DRIVE_BYTES
{
        IOCTL_HEADER	ioctl;		* IOCTL header *
        U8		numBytes,		* number of bytes read *
                        buff[128];		* read buffer *
} IOCTL_I_DRIVE_BYTES;*/

typedef struct _IOCTL_I_DEVICE_STATUS {
  IOCTL_HEADER ioctl; /* IOCTL header */
  U32 status;         /* device status */
                      /* bit 0		0,door closed
                                         1,door open
                         bit 1		0,door locked
                                         1,door unlocked
                         bit 2		0,cooked reading only
                                         1,raw and cooked reading
                         bit 3		0,read only
                                         1,read/write
                         bit 4		0,data read only
                                         1,data read and plays audio/video tracks
                         bit 5		0,no interleaving
                                         1,supports ISO-9660 interleaving size & skip factor
                         bit 6		reserved
                         bit 7		0,no prefetching
                                         1,supports prefetching requests
                         bit 8		0,no audio channel manipulation
                                         1,supports audio channel manipulation
                         bit 9		0,supports HSG addressing mode
                                         1,supports HSG and Red Book addressing
                         bit 10	reserved
                         bit 11		0,disc in drive
                                         1,no disc in drive
                         bit 12		0,doesn't support R/W sub-channels
                                         1,does support R/W sub-channels
                         bit 13-31  reserved, 0		*/
} IOCTL_I_DEVICE_STATUS;

typedef struct _IOCTL_I_SECTOR_SIZE {
  IOCTL_HEADER ioctl;  /* IOCTL header */
  CD_SECTOR_SIZE info; /* sector size information */
} IOCTL_I_SECTOR_SIZE;

typedef struct _IOCTL_I_VOLUME_SIZE {
  IOCTL_HEADER ioctl; /* IOCTL header */
  U32 sectors;        /* # sectors on device */
} IOCTL_I_VOLUME_SIZE;

typedef struct _IOCTL_I_MEDIA_CHANGED {
  IOCTL_HEADER ioctl; /* IOCTL header */
  U8 changed;         /* has the media changed? */
                      /* 1, not changed
                         0, don't know
                         0xff, changed */
} IOCTL_I_MEDIA_CHANGED;

typedef struct _IOCTL_I_AUDIO_DISC {
  IOCTL_HEADER ioctl; /* IOCTL header */
  CD_DISC_INFO info;  /* audio disc info */
} IOCTL_I_AUDIO_DISC;

typedef struct _IOCTL_I_AUDIO_TRACK {
  IOCTL_HEADER ioctl; /* IOCTL header */
  CD_TRACK_INFO info; /* audio track info */
} IOCTL_I_AUDIO_TRACK;

typedef struct _IOCTL_I_AUDIO_Q_CHANNEL {
  IOCTL_HEADER ioctl; /* IOCTL header */
  CD_Q_CHANNEL info;  /* Q channel info */
} IOCTL_I_AUDIO_Q_CHANNEL;

typedef struct _IOCTL_I_UPC_CODE {
  IOCTL_HEADER ioctl; /* IOCTL header */
  I8 control,         /* control & ADR byte */
      UPC[7],         /* UPC/EAN code,
                      low order nibble of byte 7 is 0 */
      zero,           /* ?? */
      aframe;         /* ?? */
} IOCTL_I_UPC_CODE;

typedef struct _IOCTL_I_AUDIO_STATUS {
  IOCTL_HEADER ioctl;   /* IOCTL header */
  CD_AUDIO_STATUS info; /* audio status info */
} IOCTL_I_AUDIO_STATUS;

/* IOCTL_O_EJECT is a bare IOCTL_HEADER packet */
/* IOCTL_O_RESET is a bare IOCTL_HEADER packet */
/* IOCTL_O_CLOSE_TRAY is a bare IOCTL_HEADER packet */

typedef struct _IOCTL_O_DOOR_CONTROL {
  IOCTL_HEADER ioctl; /* IOCTL header */
  U8 lock;            /* lock the door?
                      0, unlock
                      1, lock */
} IOCTL_O_DOOR_CONTROL;

/* used for DEV_READ_LONG and DEV_READ_LONG_PRE, the prefectch call
        requires that the transfer address "transAddr" be NULL */
typedef struct _DEVP_READ_LONG {
  DEV_REQ_HEADER devReq; /* device request header */
  U8 addrMode;           /* addressing mode */
                         /* 0, HSG addressing mode
                         1, Red Book addressing mode
                         2-255 - reserved */
  void _far *transAddr;  /* transfer address */
  U16 numSector;         /* number of sectors to read */
  CD_ADDR start;         /* starting address */
  U8 readMode,           /* data read mode: Cooked/Raw */
                         /* 0, cooked read mode
                                 1, raw read mode
                                 2-255 - reserved */
      interSize,         /* interleave size */
      interSkip;         /* interleave skip factor */
} DEVP_READ_LONG;

typedef struct _DEVP_SEEK {
  DEV_REQ_HEADER devReq; /* device request header */
  U8 addrMode;           /* addressing mode */
  void _far *transAddr;  /* transfer address (NULL) */
  U16 numSector;         /* number of sectors to read (0) */
  CD_ADDR start;         /* starting address */
} DEVP_SEEK;

typedef struct _DEVP_AUDIO_PLAY {
  DEV_REQ_HEADER devReq; /* device request header */
  U8 addrMode;           /* addressing mode */
  CD_ADDR start;         /* starting address */
  U32 numSector;         /* number of sectors to read (0) */
  /* possible that numSector can be sent in redBook format? */
} DEVP_AUDIO_PLAY;

/* DEVP_AUDIO_STOP is a bare DEV_REQ_HEADER packet */
/* DEVP_AUDIO_RESUME is a bare DEV_REQ_HEADER packet */
/* DEVP_INPUT_FLUSH is a bare DEV_REQ_HEADER packet */
/* DEVP_DEVICE_CLOSE is a bare DEV_REQ_HEADER packet */
/* DEVP_DEVICE_OPEN is a bare DEV_REQ_HEADER packet */
#pragma pack()

/* look-up table numBytes in CD-ROM IOCTL Input request  */
U16 IOCTLInputBytes[] = {5, 6, 0, 0, 9, 130, 5, 4, 5, 2, 7, 7, 11, 13, 11, 11};
/* look-up table numBytes in CD-ROM IOCTL Output request  */
U16 IOCTLOutputBytes[] = {1, 2, 1, 9, 0, 1};

void(_far *CDROMStrategy)(void);
void(_far *CDROMInterrupt)(void);

/* monitor this while calling Q-Info to check for busy */
U16 CDROM_STATUS;

char *DEV_ERRORS[] = {"No error",
                      "Write-protect violation",
                      "Unknown unit",
                      "Drive not ready",
                      "Unknown command",
                      "CRC error",
                      "Bad drive request structure length",
                      "Seek error",
                      "Unknown media",
                      "Sector not found",
                      "Printer out of paper",
                      "Write fault",
                      "Read fault",
                      "General failure",
                      "reserved",
                      "reserved",
                      "Invalid disk change"};

DEV_HEADER _far *DevHeaderLocate(char *name) {
  union REGS r; /* registers for general use */
  struct SREGS segs;
  DEV_HEADER _far *hdr; /* device list pointer */
  char temp[10];
  int lp;
  r.h.ah = 0x52; /* get DOS internal address */
  int86x(0x21, &r, &r, &segs);
  FP_SEG(hdr) = segs.es; /* point hdr at first device */
  FP_OFF(hdr) = r.x.bx + 0x22;
  do {
    if (hdr->attributes & 0x8000) /* have a device? */
    {                             /* copy name to temp, device names */
                                  /*  are not NULL terminated */
      _fmemcpy((char _far *)temp, hdr->name, 8);
      lp = 8;       /* NULL terminate and get rid of */
      temp[lp] = 0; /*  trailing spaces */
      while (--lp)
        if (temp[lp] == 32) temp[lp] = 0;
      if (!stricmp(name, temp)) /* compare name */
        return hdr;
    }
    hdr = hdr->next;
  } while (FP_OFF(hdr) != 0xFFFF); /* keep going til no more */
  return NULL;
}

int DevError(DEV_REQ_HEADER *header) {
  CDROM_STATUS = header->status;
  if (CDROM_STATUS & STATUS_ERROR) return STATUS_CODE(CDROM_STATUS) + 1;
  return 0;
}

void DevReqHeaderFill(DEV_REQ_HEADER *hdr, U8 command) {
  DEV_REQ_HEADER _far *header = (DEV_REQ_HEADER _far *)hdr;
  hdr->length = sizeof(DEV_REQ_HEADER);
  hdr->subUnit = 0;
  hdr->command = command;
  _asm les bx, header _asm call CDROMStrategy _asm call CDROMInterrupt
}

void DevIOCTLFill(IOCTL_HEADER *ioctl) {
  ioctl->media = 0;
  ioctl->transAddr = (void _far *)&ioctl->code;
  ioctl->startSector = 0;
  ioctl->volume = NULL;
}

void DevIOCTLInputFill(IOCTL_HEADER *ioctl, U8 code) {
  DevIOCTLFill(ioctl);
  ioctl->numBytes = IOCTLInputBytes[code];
  ioctl->code = code;
  DevReqHeaderFill(&ioctl->devReq, DEV_IOCTL_INPUT);
}

void DevIOCTLOutputFill(IOCTL_HEADER *ioctl, U8 code) {
  DevIOCTLFill(ioctl);
  ioctl->numBytes = IOCTLOutputBytes[code];
  ioctl->code = code;
  DevReqHeaderFill(&ioctl->devReq, DEV_IOCTL_OUTPUT);
}

int CDROMInit(char *name) {
  DEV_HEADER _far *rval;
  rval = DevHeaderLocate(name);
  if (rval) {
    if (rval->attributes & ~0xC800) return 2;
    CDROMStrategy = ((U8 _far *)rval + rval->strategyOffset);
    CDROMInterrupt = ((U8 _far *)rval + rval->interruptOffset);
    return 0;
  } else
    return 1;
}

int CDROMOpen(void) {
  DEV_REQ_HEADER note;
  DevReqHeaderFill(&note, DEV_DEVICE_OPEN);
  return DevError(&note);
}

int CDROMClose(void) {
  DEV_REQ_HEADER note;
  DevReqHeaderFill(&note, DEV_DEVICE_CLOSE);
  return DevError(&note);
}

int CDROMStop(void) {
  DEV_REQ_HEADER note;
  DevReqHeaderFill(&note, DEV_AUDIO_STOP);
  return DevError(&note);
}

int CDROMResume(void) {
  DEV_REQ_HEADER note;
  DevReqHeaderFill(&note, DEV_AUDIO_RESUME);
  return DevError(&note);
}

void _far *CDROMHeader(void) {
  IOCTL_I_HEADER_ADDRESS note;
  DevIOCTLInputFill(&note.ioctl, IOCTL_IN_HEADER_ADDRESS);
  return note.addr;
}

int CDROMStatus(U32 *status) {
  IOCTL_I_DEVICE_STATUS note;
  DevIOCTLInputFill(&note.ioctl, IOCTL_IN_DEVICE_STATUS);
  *status = note.status;
  return DevError(&note.ioctl.devReq);
}

int CDROMHeadLocation(CD_HEAD_LOCATION *where) {
  IOCTL_I_HEAD_LOCATION note;
  DevIOCTLInputFill(&note.ioctl, IOCTL_IN_HEAD_LOCATION);
  *where = note.info;
  return DevError(&note.ioctl.devReq);
}

int CDROMAudioChannels(CD_AUDIO_CHANNELS *where) {
  IOCTL_AUDIO_CHANNELS note;
  DevIOCTLInputFill(&note.ioctl, IOCTL_IN_AUDIO_CHANNELS);
  *where = note.info;
  return DevError(&note.ioctl.devReq);
}

int CDROMSectorSize(CD_SECTOR_SIZE *where) {
  IOCTL_I_SECTOR_SIZE note;
  DevIOCTLInputFill(&note.ioctl, IOCTL_IN_SECTOR_SIZE);
  *where = note.info;
  return DevError(&note.ioctl.devReq);
}

int CDROMSectorInfo(U32 *sectors) {
  IOCTL_I_VOLUME_SIZE note;
  DevIOCTLInputFill(&note.ioctl, IOCTL_IN_VOLUME_SIZE);
  *sectors = note.sectors;
  return DevError(&note.ioctl.devReq);
}

int CDROMMediaChanged(U8 *change) {
  IOCTL_I_MEDIA_CHANGED note;
  DevIOCTLInputFill(&note.ioctl, IOCTL_IN_MEDIA_CHANGED);
  if (note.changed == 1)
    *change = 0;
  else
    *change = 1;
  return DevError(&note.ioctl.devReq);
}

int CDROMDiscInfo(CD_DISC_INFO *where) {
  IOCTL_I_AUDIO_DISC note;
  DevIOCTLInputFill(&note.ioctl, IOCTL_IN_AUDIO_DISC);
  *where = note.info;
  return DevError(&note.ioctl.devReq);
}

int CDROMTrackInfo(CD_TRACK_INFO *where) {
  IOCTL_I_AUDIO_TRACK note;
  note.info.track = where->track;
  DevIOCTLInputFill(&note.ioctl, IOCTL_IN_AUDIO_TRACK);
  *where = note.info;
  return DevError(&note.ioctl.devReq);
}

int CDROMAudioQChannel(CD_Q_CHANNEL *where) {
  IOCTL_I_AUDIO_Q_CHANNEL note;
  DevIOCTLInputFill(&note.ioctl, IOCTL_IN_AUDIO_Q_CHANNEL);
  *where = note.info;
  return DevError(&note.ioctl.devReq);
}

int CDROMUPCCode(char *where) {
  U16 lp;
  IOCTL_I_UPC_CODE note;
  DevIOCTLInputFill(&note.ioctl, IOCTL_IN_UPC_CODE);
  if (!note.control)
    strcpy(where, "Unknown");
  else {
    for (lp = 0; lp < 7; lp++) {
      *where++ = (char)(((note.UPC[lp] & 0xf0) >> 4) + '0');
      *where++ = (char)((note.UPC[lp] & 0x0f) + '0');
    }
    *where++ = (char)(((note.UPC[lp] & 0xf0) >> 4) + '0');
    *where = 0;
  }
  return DevError(&note.ioctl.devReq);
}

int CDROMAudioStatus(CD_AUDIO_STATUS *where) {
  IOCTL_I_AUDIO_STATUS note;
  DevIOCTLInputFill(&note.ioctl, IOCTL_IN_AUDIO_STATUS);
  *where = note.info;
  return DevError(&note.ioctl.devReq);
}

int CDROMEjectDisc(void) {
  IOCTL_HEADER note;
  DevIOCTLOutputFill(&note, IOCTL_OUT_EJECT_DISC);
  return DevError(&note.devReq);
}

int CDROMReset(void) {
  IOCTL_HEADER note;
  DevIOCTLOutputFill(&note, IOCTL_OUT_RESET);
  return DevError(&note.devReq);
}

int CDROMCloseTray(void) {
  IOCTL_HEADER note;
  DevIOCTLOutputFill(&note, IOCTL_OUT_CLOSE_TRAY);
  return DevError(&note.devReq);
}

int CDROMAudioControl(CD_AUDIO_CHANNELS *where) {
  IOCTL_AUDIO_CHANNELS note;
  note.info = *where;
  DevIOCTLOutputFill(&note.ioctl, IOCTL_OUT_AUDIO_CONTROL);
  return DevError(&note.ioctl.devReq);
}

int CDROMDoorControl(U8 lock) {
  IOCTL_O_DOOR_CONTROL note;
  note.lock = lock;
  DevIOCTLOutputFill(&note.ioctl, IOCTL_OUT_DOOR_CONTROL);
  return DevError(&note.ioctl.devReq);
}

int CDROMSeek(U8 addrMode, CD_ADDR start) {
  DEVP_SEEK note;
  note.addrMode = addrMode;
  note.transAddr = NULL;
  note.numSector = 0;
  note.start = start;
  DevReqHeaderFill(&note.devReq, DEV_SEEK);
  return DevError(&note.devReq);
}

int CDROMPlay(U8 addrMode, CD_ADDR start, U32 numSector) {
  DEVP_AUDIO_PLAY note;
  note.addrMode = addrMode;
  note.start = start;
  note.numSector = numSector;
  DevReqHeaderFill(&note.devReq, DEV_AUDIO_PLAY);
  return DevError(&note.devReq);
}

int CDROMRead(U8 addrMode, CD_ADDR start, U16 numSector, U8 readMode,
              void *transAddr) {
  DEVP_READ_LONG note;
  note.addrMode = addrMode;
  note.start = start;
  note.readMode = readMode;
  note.numSector = numSector;
  note.transAddr = (void _far *)transAddr;
  note.interSize = 0;
  note.interSkip = 0;
  DevReqHeaderFill(&note.devReq, DEV_READ_LONG);
  return DevError(&note.devReq);
}

/*int CDROMFlush(void)
{
        DEV_REQ_HEADER note;
        DevReqHeaderFill(&note,DEV_INPUT_FLUSH);
        return note.status & STATUS_ERROR;
}*/

void printtrackinfo(CD_TRACK_INFO *info) {
  printf(" track  %2d:    %2d:%02d.%02d\n", info->track, info->start.min,
         info->start.sec, info->start.frame);
}

void printdiscinfo(CD_DISC_INFO *info) {
  printf("Disk has tracks %d through %d\n", info->lowTrack, info->highTrack);
}

void printqinfo(CD_Q_CHANNEL *qinfo) {
  printf("%-2d  %-2d  %2d:%02d.%02d  %2d:%02d.%02d\r", qinfo->track,
         qinfo->index, qinfo->min, qinfo->sec, qinfo->frame, qinfo->amin,
         qinfo->asec, qinfo->aframe);
}

int CDROMSetup(void) {
  CD_TRACK_INFO track;
  CD_DISC_INFO disc;
  CD_Q_CHANNEL qinfo;
  U32 temp;
  CD_ADDR start;
  int tracknum;

  if (CDROMInit("MSCD001")) {
    puts("CDROMInit failed");
    return 0;
  }

  CDROMStop();
  return 1;
}

void CDROMSetTrack(int i) {
  CD_TRACK_INFO track;
  U32 temp;
  CD_ADDR start;
  int tracknum;

  tracknum = i;

  track.track = tracknum + 1;
  CDROMTrackInfo(&track);

  temp = RED_TO_HSG(track.start);

  track.track = tracknum;
  CDROMTrackInfo(&track);
  temp -= RED_TO_HSG(track.start);
  start.redBook = track.start;

  CDROMPlay(CD_ADDR_RED_BOOK, start, temp);
  CDROMStop();
}
