PROJ = gamma
PROJFILE = gamma.mak
DEBUG = 0

PWBRMAKE  = pwbrmake
NMAKEBSC1  = set
NMAKEBSC2  = nmake
CC  = cl
CFLAGS_G  = /G2 /BATCH
CFLAGS_D  = /Od /Zi /Gi$(PROJ).mdt
CFLAGS_R  = /Ot /Ol /Og /Oe /Oi /Gs
MAPFILE_D  = NUL
MAPFILE_R  = NUL
LFLAGS_G  =  /NOI  /BATCH
LFLAGS_D  = /CO /FAR /PACKC
LFLAGS_R  =  /EXE /FAR /PACKC
LINKER	= link
ILINK  = ilink
LRF  = echo > NUL
CVFLAGS  =  /25

OBJS  = GAMMA.obj GAMMA2.obj GAMMA3.obj CD.obj GLIB.obj NEWCONFI.obj\
	PODPERF.obj SETUP.obj SETHOST.obj cdromlib.obj outctl.obj
LIBS_EXT  = asd.lib ..\comm\comm_s.lib
LIBS  = $(LIBS_EXT)
SBRS  = GAMMA.sbr GAMMA2.sbr GAMMA3.sbr CD.sbr GLIB.sbr NEWCONFI.sbr\
	PODPERF.sbr SETUP.sbr SETHOST.sbr cdromlib.sbr outctl.sbr

all: $(PROJ).exe

.SUFFIXES:
.SUFFIXES: .sbr .obj .c

GAMMA.obj : GAMMA.C video.h asd.h gamma.h asddat.h asdmenu.h outctl.h g_asd.h\
	fontop.h photon.h phoproto.h gamma2.h

GAMMA.sbr : GAMMA.C video.h asd.h gamma.h asddat.h asdmenu.h outctl.h g_asd.h\
	fontop.h photon.h phoproto.h gamma2.h

GAMMA2.obj : GAMMA2.C video.h gamma.h asd.h asddat.h outctl.h g_asd.h fontop.h\
	photon.h phoproto.h gamma2.h

GAMMA2.sbr : GAMMA2.C video.h gamma.h asd.h asddat.h outctl.h g_asd.h fontop.h\
	photon.h phoproto.h gamma2.h

GAMMA3.obj : GAMMA3.C video.h asd.h gamma.h outctl.h photon.h phoproto.h\
	gamma2.h

GAMMA3.sbr : GAMMA3.C video.h asd.h gamma.h outctl.h photon.h phoproto.h\
	gamma2.h

CD.obj : CD.C gamma.h cdromf.h photon.h phoproto.h gamma2.h

CD.sbr : CD.C gamma.h cdromf.h photon.h phoproto.h gamma2.h

GLIB.obj : GLIB.C gamma.h glib.h photon.h phoproto.h gamma2.h

GLIB.sbr : GLIB.C gamma.h glib.h photon.h phoproto.h gamma2.h

NEWCONFI.obj : NEWCONFI.C gamma.h asd.h asdmenu.h video.h photon.h phoproto.h\
	gamma2.h

NEWCONFI.sbr : NEWCONFI.C gamma.h asd.h asdmenu.h video.h photon.h phoproto.h\
	gamma2.h

PODPERF.obj : PODPERF.C gamma.h photon.h phoproto.h gamma2.h

PODPERF.sbr : PODPERF.C gamma.h photon.h phoproto.h gamma2.h

SETUP.obj : SETUP.C gamma.h photon.h phoproto.h gamma2.h

SETUP.sbr : SETUP.C gamma.h photon.h phoproto.h gamma2.h

SETHOST.obj : SETHOST.C gamma.h photon.h phoproto.h gamma2.h

SETHOST.sbr : SETHOST.C gamma.h photon.h phoproto.h gamma2.h

cdromlib.obj : cdromlib.c cdrom.h

cdromlib.sbr : cdromlib.c cdrom.h

outctl.obj : outctl.c outctl.h asddat.h video.h ..\comm\comm.h asd.h

outctl.sbr : outctl.c outctl.h asddat.h video.h ..\comm\comm.h asd.h


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
