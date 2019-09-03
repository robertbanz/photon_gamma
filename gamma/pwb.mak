PROJ = GAMMA
PROJFILE = pwb.mak
DEBUG = 0

PWBRMAKE  = pwbrmake
NMAKEBSC1  = set
NMAKEBSC2  = nmake
CC  = cl
CFLAGS_G  = /W2 /G2 /Zp /BATCH
CFLAGS_D  = /Gi.\$(PROJ).mdt /Zi /Od
CFLAGS_R  = /Ot /Oi /Ol /Oe /Og /Gs
MAPFILE_D  = NUL
MAPFILE_R  = NUL
LFLAGS_G  = /NOI /BATCH
LFLAGS_D  = /CO /FAR /PACKC /PACKD /PMTYPE:VIO
LFLAGS_R  = /EXE /FAR /PACKC /PACKD /PMTYPE:VIO
LINKER	= link
ILINK  = ilink
LRF  = echo > NUL
LLIBS_R  =  /NOD:SLIBCE SLIBCEP
LLIBS_D  =  /NOD:SLIBCE SLIBCEP

OBJS  = .\GAMMA.obj
SBRS  = .\GAMMA.sbr

all: .\$(PROJ).exe

.SUFFIXES:
.SUFFIXES: .sbr .obj .c

.\GAMMA.obj : GAMMA.c
!IF $(DEBUG)
	$(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo.\GAMMA.obj GAMMA.c
!ELSE
	$(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo.\GAMMA.obj GAMMA.c
!ENDIF

.\GAMMA.sbr : GAMMA.c
!IF $(DEBUG)
	$(CC) /Zs $(CFLAGS_G) $(CFLAGS_D) /FR.\GAMMA.sbr GAMMA.c
!ELSE
	$(CC) /Zs $(CFLAGS_G) $(CFLAGS_R) /FR.\GAMMA.sbr GAMMA.c
!ENDIF


.\$(PROJ).bsc : $(SBRS)
	$(PWBRMAKE) @<<
$(BRFLAGS) $(SBRS)
<<

.\$(PROJ).exe : $(OBJS)
!IF $(DEBUG)
	$(LRF) @<<.\$(PROJ).lrf
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
	$(LRF) @<<.\$(PROJ).lrf
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
	$(LINKER) @.\$(PROJ).lrf


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


run: .\$(PROJ).exe
	.\$(PROJ).exe $(RUNFLAGS)

debug: .\$(PROJ).exe
	CVP $(CVFLAGS) .\$(PROJ).exe $(RUNFLAGS)
