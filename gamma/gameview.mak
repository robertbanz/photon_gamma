PROJ = GAMEVIEW
PROJFILE = GAMEVIEW.MAK
DEBUG = 0

PWBRMAKE  = pwbrmake
NMAKEBSC1  = set
NMAKEBSC2  = nmake
CC  = cl
CFLAGS_G  = /W2 /BATCH
CFLAGS_D  = /qc /Gi$(PROJ).mdt /Zr /Zi /Od
CFLAGS_R  = /Ot /Oi /Ol /Oe /Og /Gs
MAPFILE_D  = NUL
MAPFILE_R  = NUL
LFLAGS_G  = /NOI /BATCH
LFLAGS_D  = /CO /INC /FAR /PACKC
LFLAGS_R  = /EXE /FAR /PACKC
LINKER  = link
ILINK  = ilink
LRF  = echo > NUL

OBJS	= GAMEVIEW.obj ASDS.obj ASDMENUD.obj
SBRS	= GAMEVIEW.sbr ASDS.sbr ASDMENUD.sbr

all: $(PROJ).exe

.SUFFIXES:
.SUFFIXES: .sbr .obj .c

GAMEVIEW.obj : GAMEVIEW.C C:\MSC\INCLUDE\stdio.h C:\MSC\INCLUDE\stdlib.h\
		  C:\MSC\INCLUDE\time.h C:\MSC\INCLUDE\dos.h C:\MSC\INCLUDE\string.h\
		  C:\MSC\INCLUDE\conio.h video.h asd.h asdmenu.h

GAMEVIEW.sbr : GAMEVIEW.C C:\MSC\INCLUDE\stdio.h C:\MSC\INCLUDE\stdlib.h\
		  C:\MSC\INCLUDE\time.h C:\MSC\INCLUDE\dos.h C:\MSC\INCLUDE\string.h\
		  C:\MSC\INCLUDE\conio.h video.h asd.h asdmenu.h

ASDS.obj : ASDS.C C:\MSC\INCLUDE\stdio.h C:\MSC\INCLUDE\string.h\
		  C:\MSC\INCLUDE\stdarg.h C:\MSC\INCLUDE\malloc.h C:\MSC\INCLUDE\bios.h\
		  C:\MSC\INCLUDE\conio.h video.h asddat.h asd.h

ASDS.sbr : ASDS.C C:\MSC\INCLUDE\stdio.h C:\MSC\INCLUDE\string.h\
		  C:\MSC\INCLUDE\stdarg.h C:\MSC\INCLUDE\malloc.h C:\MSC\INCLUDE\bios.h\
		  C:\MSC\INCLUDE\conio.h video.h asddat.h asd.h

ASDMENUD.obj : ASDMENUD.C C:\MSC\INCLUDE\memory.h C:\MSC\INCLUDE\string.h\
		  C:\MSC\INCLUDE\stdio.h C:\MSC\INCLUDE\malloc.h asd.h video.h\
		  asdmenud.h asdmenu.h

ASDMENUD.sbr : ASDMENUD.C C:\MSC\INCLUDE\memory.h C:\MSC\INCLUDE\string.h\
		  C:\MSC\INCLUDE\stdio.h C:\MSC\INCLUDE\malloc.h asd.h video.h\
		  asdmenud.h asdmenu.h


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
!IF $(DEBUG)
		  $(ILINK) -a -e "$(LINKER) @$(PROJ).lrf" $@
!ELSE
		  $(LINKER) @$(PROJ).lrf
!ENDIF


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
