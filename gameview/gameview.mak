PROJ = gameview
PROJFILE = gameview.mak
DEBUG = 0

PWBRMAKE  = pwbrmake
NMAKEBSC1  = set
NMAKEBSC2  = nmake
CC  = cl
CFLAGS_G  = /W2 /BATCH
CFLAGS_D  = /Gi$(PROJ).mdt /Zi /Od
CFLAGS_R  = /Ot /Oi /Ol /Oe /Og /Gs
MAPFILE_D  = NUL
MAPFILE_R  = NUL
LFLAGS_G  = /NOI /BATCH
LFLAGS_D  = /CO /FAR /PACKC
LFLAGS_R  = /EXE /FAR /PACKC
LINKER	= link
ILINK  = ilink
LRF  = echo > NUL
LLIBS_R  =  /NOD:SLIBCE SLIBCER
LLIBS_D  =  /NOD:SLIBCE SLIBCER

OBJS_EXT  = VIDEOD.OBJ
OBJS  = GAMEVIEW.obj VIDEO.obj ASD.obj ASDMENU.obj $(OBJS_EXT)
SBRS  = GAMEVIEW.sbr VIDEO.sbr ASD.sbr ASDMENU.sbr

all: $(PROJ).exe

.SUFFIXES:
.SUFFIXES: .obj .sbr .c

GAMEVIEW.obj : GAMEVIEW.C asd.h asdmenud.h video.h

GAMEVIEW.sbr : GAMEVIEW.C asd.h asdmenud.h video.h

VIDEO.obj : VIDEO.C video.h videodat.h

VIDEO.sbr : VIDEO.C video.h videodat.h

ASD.obj : ASD.C video.h asddat.h asd.h

ASD.sbr : ASD.C video.h asddat.h asd.h

ASDMENU.obj : ASDMENU.C video.h asd.h asdmenud.h asdmenu.h

ASDMENU.sbr : ASDMENU.C video.h asd.h asdmenud.h asdmenu.h


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


.c.obj :
!IF $(DEBUG)
	$(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo$@ $<
!ELSE
	$(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo$@ $<
!ENDIF

.c.sbr :
!IF $(DEBUG)
	$(CC) /Zs $(CFLAGS_G) $(CFLAGS_D) /FR$@ $<
!ELSE
	$(CC) /Zs $(CFLAGS_G) $(CFLAGS_R) /FR$@ $<
!ENDIF


run: $(PROJ).exe
	$(PROJ).exe $(RUNFLAGS)

debug: $(PROJ).exe
	CV $(CVFLAGS) $(PROJ).exe $(RUNFLAGS)
