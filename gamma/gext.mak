PROJ = gext
PROJFILE = gext.mak
DEBUG = 0

PWBRMAKE  = pwbrmake
NMAKEBSC1  = set
NMAKEBSC2  = nmake
CC  = cl
CFLAGS_G  = /W2 /DEXTENDED_GRAPHICS /BATCH
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

OBJS  = GAMMA.obj GAMMA2.obj GAMMA3.obj CD.obj GLIB.obj NEWCONFI.obj\
	PODPERF.obj SETUP.obj SETHOST.obj
LIBS_EXT  = gasd.lib
LIBS  = $(LIBS_EXT)
SBRS  = GAMMA.sbr GAMMA2.sbr GAMMA3.sbr CD.sbr GLIB.sbr NEWCONFI.sbr\
	PODPERF.sbr SETUP.sbr SETHOST.sbr

all: $(PROJ).exe

.SUFFIXES:
.SUFFIXES: .sbr .obj .c

GAMMA.obj : GAMMA.C video.h asd.h gamma.h asddat.h g_asd.h fontop.h photon.h\
	phoproto.h

GAMMA.sbr : GAMMA.C video.h asd.h gamma.h asddat.h g_asd.h fontop.h photon.h\
	phoproto.h

GAMMA2.obj : GAMMA2.C video.h gamma.h asd.h asddat.h g_asd.h fontop.h photon.h\
	phoproto.h

GAMMA2.sbr : GAMMA2.C video.h gamma.h asd.h asddat.h g_asd.h fontop.h photon.h\
	phoproto.h

GAMMA3.obj : GAMMA3.C video.h asd.h gamma.h photon.h phoproto.h

GAMMA3.sbr : GAMMA3.C video.h asd.h gamma.h photon.h phoproto.h

CD.obj : CD.C gamma.h photon.h phoproto.h

CD.sbr : CD.C gamma.h photon.h phoproto.h

GLIB.obj : GLIB.C gamma.h glib.h photon.h phoproto.h

GLIB.sbr : GLIB.C gamma.h glib.h photon.h phoproto.h

NEWCONFI.obj : NEWCONFI.C gamma.h asd.h asdmenu.h video.h photon.h phoproto.h

NEWCONFI.sbr : NEWCONFI.C gamma.h asd.h asdmenu.h video.h photon.h phoproto.h

PODPERF.obj : PODPERF.C gamma.h photon.h phoproto.h

PODPERF.sbr : PODPERF.C gamma.h photon.h phoproto.h

SETUP.obj : SETUP.C gamma.h photon.h phoproto.h

SETUP.sbr : SETUP.C gamma.h photon.h phoproto.h

SETHOST.obj : SETHOST.C gamma.h photon.h phoproto.h

SETHOST.sbr : SETHOST.C gamma.h photon.h phoproto.h


$(PROJ).bsc : $(SBRS)
	$(PWBRMAKE) @<<
$(BRFLAGS) $(SBRS)
<<

$(PROJ).exe : $(OBJS) $(LIBS)
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
	CV $(CVFLAGS) $(PROJ).exe $(RUNFLAGS)
