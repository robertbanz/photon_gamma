PROJ = GAMADOOR
PROJFILE = GAMADOOR.MAK
DEBUG = 0

PWBRMAKE  = pwbrmake
NMAKEBSC1  = set
NMAKEBSC2  = nmake
CC  = cl
CFLAGS_G  =  /BATCH
CFLAGS_D  = /Gi$(PROJ).mdt /Zi /Od
CFLAGS_R  = /Ot /Ol /Og /Oe /Oi
MAPFILE_D  = NUL
MAPFILE_R  = NUL
LFLAGS_G  =  /NOI  /BATCH
LFLAGS_D  = /CO /FAR /PACKC /PACKD /PMTYPE:VIO
LFLAGS_R  =  /EXE /FAR /PACKC /PACKD
LINKER	= link
ILINK  = ilink
LRF  = echo > NUL
LLIBS_R  =  /NOD:SLIBCE SLIBCER
LLIBS_D  =  /NOD:SLIBCE SLIBCER

OBJS  = ASDMENUD.obj ASDS.obj GAMEVIEW.obj
SBRS  = ASDMENUD.sbr ASDS.sbr GAMEVIEW.sbr

all: $(PROJ).exe

.SUFFIXES:
.SUFFIXES: .sbr .obj .c

ASDMENUD.obj : ASDMENUD.C asd.h video.h asdmenud.h asdmenu.h

ASDMENUD.sbr : ASDMENUD.C asd.h video.h asdmenud.h asdmenu.h

ASDS.obj : ASDS.C video.h asddat.h asd.h

ASDS.sbr : ASDS.C video.h asddat.h asd.h

GAMEVIEW.obj : GAMEVIEW.C asd.h asdmenud.h video.h

GAMEVIEW.sbr : GAMEVIEW.C asd.h asdmenud.h video.h


$(PROJ).bsc : $(SBRS)
	$(PWBRMAKE) @<<
$(BRFLAGS) $(SBRS)
<<

$(PROJ).exe : $(OBJS)
!IF $(DEBUG)
	$(LRF) @<<$(PROJ).lrf
$(RT_OBJS: = +^
) $(OBJS: = +^
)
$@
$(MAPFILE_D)
$(LLIBS_G: = +^
) +
$(LLIBS_D: = +^
) +
$(LIBS: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_D);
<<
!ELSE
	$(LRF) @<<$(PROJ).lrf
$(RT_OBJS: = +^
) $(OBJS: = +^
)
$@
$(MAPFILE_R)
$(LLIBS_G: = +^
) +
$(LLIBS_R: = +^
) +
$(LIBS: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_R);
<<
!ENDIF
	$(LINKER) @$(PROJ).lrf


.c.sbr :
!IF $(DEBUG)
	$(CC) /Zs $(CFLAGS_G) $(CFLAGS_D) /FR$@ $<
!ELSE
	$(CC) /Zs $(CFLAGS_G) $(CFLAGS_R) /FR$@ $<
!ENDIF

.c.obj :
!IF $(DEBUG)
	$(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo$@ $<
!ELSE
	$(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo$@ $<
!ENDIF


run: $(PROJ).exe
	$(PROJ).exe $(RUNFLAGS)

debug: $(PROJ).exe
	CVP $(CVFLAGS) $(PROJ).exe $(RUNFLAGS)
