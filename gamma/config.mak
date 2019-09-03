PROJ = config
PROJFILE = config.mak
DEBUG = 0

CC  = cl
CFLAGS_G  =  /BATCH
CFLAGS_D  = /qc /Gi.\$(PROJ).mdt /Zr /Zi /Od
CFLAGS_R  = /Ot
MAPFILE_D  = NUL
MAPFILE_R  = NUL
LFLAGS_G  = /NOI /BATCH
LFLAGS_D  = /CO /INC /FAR /PACKC
LFLAGS_R  = /EXE /FAR /PACKC
LINKER	= link
ILINK  = ilink
LRF  = echo > NUL

OBJS_EXT  = VIDEOD.OBJ
OBJS  = .\CONFIG.obj .\NEWCONFI.obj .\VIDEO.obj .\ASD.obj .\ASDMENU.obj\
	$(OBJS_EXT)

all: .\$(PROJ).exe

.SUFFIXES:
.SUFFIXES: .obj .c

.\CONFIG.obj : CONFIG.C gamma.h asd.h asdmenu.h video.h videodat.h photon.h\
	phoproto.h
!IF $(DEBUG)
	$(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo.\CONFIG.obj CONFIG.C
!ELSE
	$(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo.\CONFIG.obj CONFIG.C
!ENDIF

.\NEWCONFI.obj : NEWCONFI.C gamma.h asd.h asdmenu.h video.h photon.h\
	phoproto.h
!IF $(DEBUG)
	$(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo.\NEWCONFI.obj NEWCONFI.C
!ELSE
	$(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo.\NEWCONFI.obj NEWCONFI.C
!ENDIF

.\VIDEO.obj : VIDEO.C video.h videodat.h
!IF $(DEBUG)
	$(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo.\VIDEO.obj VIDEO.C
!ELSE
	$(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo.\VIDEO.obj VIDEO.C
!ENDIF

.\ASD.obj : ASD.C video.h asddat.h asd.h
!IF $(DEBUG)
	$(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo.\ASD.obj ASD.C
!ELSE
	$(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo.\ASD.obj ASD.C
!ENDIF

.\ASDMENU.obj : ASDMENU.C asd.h video.h asdmenud.h asdmenu.h
!IF $(DEBUG)
	$(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo.\ASDMENU.obj ASDMENU.C
!ELSE
	$(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo.\ASDMENU.obj ASDMENU.C
!ENDIF


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
!IF $(DEBUG)
	$(ILINK) -a -e "$(LINKER) @.\$(PROJ).lrf" $@
!ELSE
	$(LINKER) @.\$(PROJ).lrf
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
	CV $(CVFLAGS) .\$(PROJ).exe $(RUNFLAGS)
