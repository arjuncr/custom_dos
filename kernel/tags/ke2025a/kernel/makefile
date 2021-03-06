#
# Makefile for Borland C++ 3.1 for kernel.sys
#
# $Id: makefile 321 2001-11-13 23:36:45Z bartoldeman $
#

# $Log$
# Revision 1.2  2001/11/13 23:36:45  bartoldeman
# Kernel 2025a final changes.
#
# Revision 1.1  2001/11/04 20:10:15  bartoldeman
# Added new makefile names, utils sources, kconfig.h
#
# Revision 1.13  2001/07/24 16:56:29  bartoldeman
# fixes for FCBs, DJGPP ls, DBLBYTE, dyninit allocation (2024e).
#
# Revision 1.12  2001/07/22 01:58:58  bartoldeman
# Support for Brian's FORMAT, DJGPP libc compilation, cleanups, MSCDEX
#
# Revision 1.11  2001/07/09 22:19:33  bartoldeman
# LBA/FCB/FAT/SYS/Ctrl-C/ioctl fixes + memory savings
#
# Revision 1.10  2001/04/21 22:32:53  bartoldeman
# Init DS=Init CS, fixed stack overflow problems and misc bugs.
#
# Revision 1.9  2001/04/16 14:28:32  bartoldeman
# Kernel build 2024. Fixed critical error handler/config.sys/makefiles/UMBs
#
# Revision 1.8  2001/04/15 03:21:50  bartoldeman
# See history.txt for the list of fixes.
#
# Revision 1.7  2001/03/25 17:11:54  bartoldeman
# Fixed sys.com compilation. Updated to 2023. Also: see history.txt.
#
# Revision 1.6  2001/03/21 02:56:26  bartoldeman
# See history.txt for changes. Bug fixes and HMA support are the main ones.
#
# Revision 1.4  2000/08/06 05:50:17  jimtabor
# Add new files and update cvs with patches and changes
#
# Revision 1.3  2000/05/25 20:56:21  jimtabor
# Fixed project history
#
# Revision 1.2  2000/05/08 04:30:00  jimtabor
# Update CVS to 2020
#
# Revision 1.1.1.1  2000/05/06 19:34:53  jhall1
# The FreeDOS Kernel.  A DOS kernel that aims to be 100% compatible with
# MS-DOS.  Distributed under the GNU GPL.
#
# Revision 1.14  2000/03/31 05:40:09  jtabor
# Added Eric W. Biederman Patches
#
# Revision 1.13  2000/03/17 22:59:04  kernel
# Steffen Kaiser's NLS changes
#
# Revision 1.12  2000/03/09 06:07:11  kernel
# 2017f updates by James Tabor
#
# Revision 1.11  1999/09/23 04:40:47  jprice
# *** empty log message ***
#
# Revision 1.8  1999/09/13 20:41:41  jprice
# Some clean up.
#
# Revision 1.7  1999/08/25 03:18:09  jprice
# ror4 patches to allow TC 2.01 compile.
#
# Revision 1.6  1999/08/10 17:57:13  jprice
# ror4 2011-02 patch
#
# Revision 1.5  1999/04/23 04:25:15  jprice
# no message
#
# Revision 1.4  1999/04/23 03:45:11  jprice
# Improved by jprice
#
# Revision 1.3  1999/04/16 12:21:22  jprice
# Steffen c-break handler changes
#
# Revision 1.2  1999/04/13 15:48:21  jprice
# no message
#
# Revision 1.1.1.1  1999/03/29 15:41:15  jprice
# New version without IPL.SYS
#
# Revision 1.7  1999/03/01 06:04:37  jprice
# Fixed so it'll work with config.mak
#
# Revision 1.6  1999/03/01 05:46:43  jprice
# Turned off DEBUG define.
#
# Revision 1.5  1999/02/09 04:49:43  jprice
# Make makefile use common config.mak file
#
# Revision 1.4  1999/02/08 05:55:57  jprice
# Added Pat's 1937 kernel patches
#
# Revision 1.3  1999/02/04 03:09:59  jprice
# Added option to share constants (-d).
#
# Revision 1.2  1999/01/22 04:13:26  jprice
# Formating
#
# Revision 1.1.1.1  1999/01/20 05:51:01  jprice
# Imported sources
#
#
#   Rev 1.8.1 10 Jan 1999            SRM
#Took out "/P-" from TLINK
#Changed "bcc" to "tcc"
#
#   Rev 1.9   06 Dec 1998  8:45:40   patv
#Added new files for I/O subsystem.
#
#   Rev 1.8   22 Jan 1998 14:50:06   patv
#Outdated stacks.asm.
#
#   Rev 1.6   03 Jan 1998  8:36:50   patv
#Converted data area to SDA format
#
#   Rev 1.5   30 Jan 1997  7:55:54   patv
#Added TSC flag for trace support.
#
#   Rev 1.4   16 Jan 1997 12:46:42   patv
#pre-Release 0.92 feature additions
#
#   Rev 1.3   29 Aug 1996 13:07:34   patv
#Bug fixes for v0.91b
#
#   Rev 1.2   29 May 1996 21:03:32   patv
#bug fixes for v0.91a
#
#   Rev 1.1   19 Feb 1996  3:35:38   patv
#Added NLS, int2f and config.sys processing
#
#   Rev 1.0   02 Jul 1995  8:30:22   patv
#Initial revision.
#
# $EndLog$
#

!include "..\mkfiles\generic.mak"

RELEASE = 1.00

# Compiler and Options 

INCLUDE=..\hdr
LIB=    $(COMPILERBASE)\lib

LIBPATH = .
#AFLAGS      = /Mx /DSTANDALONE=1 /I..\HDR

LIBS        =..\LIB\DEVICE.LIB ..\LIB\LIBM.LIB
#INITCFLAGS =$(ALLCFLAGS) -zAINIT -zCINIT_TEXT -zDIB -zRID -zTID -zPIGROUP -zBIB \
#-zGIGROUP -zSIGROUP
#CFLAGS     =$(ALLCFLAGS) -zAHMA -zCHMA_TEXT
HDR=../hdr/

#               *Implicit Rules*
#     'standard' compiles

.c.obj :
	$(CC) $(CFLAGS) -c $*.c
	$(PATCHOBJ) $*.obj $(STDPATCH)

#               *List Macros*


EXE_dependencies =  \
 apisupt.obj  \
 rtlsupt.obj  \
 asmsupt.obj  \
 blockio.obj  \
 break.obj    \
 chario.obj   \
 config.obj   \
 console.obj  \
 dosidle.obj  \
 dosfns.obj   \
 dosnames.obj \
 dsk.obj      \
 initdisk.obj      \
 entry.obj    \
 error.obj    \
 execrh.obj   \
 fatdir.obj   \
 fatfs.obj    \
 fattab.obj   \
 fcbfns.obj   \
 initoem.obj  \
 int2f.obj    \
 inthndlr.obj \
 io.obj       \
 intr.obj     \
 ioctl.obj    \
 irqstack.obj \
 kernel.obj   \
 lfnapi.obj   \
 main.obj     \
 memmgr.obj   \
 misc.obj     \
 newstuff.obj \
 network.obj  \
 nls.obj      \
 nls_hc.obj   \
 nlssupt.obj  \
 prf.obj      \
 iprf.obj  \
 printer.obj  \
 procsupt.obj \
 serial.obj   \
 strings.obj  \
 sysclk.obj   \
 syspack.obj  \
 systime.obj  \
 task.obj     \
 inithma.obj  \
 dyndata.obj  \
 dyninit.obj

#               *Explicit Rules*

production:     ..\bin\kernel.sys

..\bin\kernel.sys: kernel.sys
		copy kernel.sys ..\bin
		copy kernel.sys ..\bin\$(THETARGET).sys
		copy kernel.map ..\bin\$(THETARGET).map

kernel.sys:	kernel.exe ..\utils\exeflat.exe
		..\utils\exeflat kernel.exe kernel.sys 0x60 -S0x10 -S0x8B

clobber:        clean
		-$(RM) kernel.exe kernel.sys status.me

clean:
		-$(RM) *.obj *.bak *.crf *.xrf *.map *.lst *.cod

# XXX: This is a very ugly way of linking the kernel, forced upon us by the
# inability of Turbo `make' 2.0 to perform command line redirection. -- ror4
kernel.exe: $(EXE_dependencies) $(LIBS) 
	-$(RM) kernel.lib
	$(LIBUTIL) kernel.lib $(LIBPLUS)entry.obj $(LIBPLUS)io.obj $(LIBPLUS)blockio.obj $(LIBPLUS)chario.obj $(LIBPLUS)dosfns.obj $(LIBPLUS)console.obj$(LIBTERM)
	$(LIBUTIL) kernel.lib $(LIBPLUS)printer.obj $(LIBPLUS)serial.obj $(LIBPLUS)dsk.obj $(LIBPLUS)initdisk.obj $(LIBPLUS)error.obj $(LIBPLUS)fatdir.obj $(LIBPLUS)fatfs.obj$(LIBTERM)
	$(LIBUTIL) kernel.lib $(LIBPLUS)fattab.obj $(LIBPLUS)fcbfns.obj $(LIBPLUS)initoem.obj $(LIBPLUS)initHMA.obj $(LIBPLUS)inthndlr.obj $(LIBPLUS)ioctl.obj $(LIBPLUS)nls_hc.obj$(LIBTERM)
	$(LIBUTIL) kernel.lib $(LIBPLUS)main.obj $(LIBPLUS)config.obj $(LIBPLUS)memmgr.obj $(LIBPLUS)misc.obj $(LIBPLUS)newstuff.obj $(LIBPLUS)nls.obj $(LIBPLUS)intr.obj$(LIBTERM)
	$(LIBUTIL) kernel.lib $(LIBPLUS)dosnames.obj $(LIBPLUS)prf.obj $(LIBPLUS)strings.obj $(LIBPLUS)network.obj $(LIBPLUS)sysclk.obj $(LIBPLUS)syspack.obj$(LIBTERM)
	$(LIBUTIL) kernel.lib $(LIBPLUS)systime.obj $(LIBPLUS)task.obj $(LIBPLUS)int2f.obj $(LIBPLUS)irqstack.obj $(LIBPLUS)apisupt.obj$(LIBTERM)
	$(LIBUTIL) kernel.lib $(LIBPLUS)asmsupt.obj $(LIBPLUS)execrh.obj $(LIBPLUS)nlssupt.obj $(LIBPLUS)procsupt.obj  $(LIBPLUS)break.obj$(LIBTERM)
	$(LIBUTIL) kernel.lib $(LIBPLUS)dosidle.obj $(LIBPLUS)dyndata.obj $(LIBPLUS)dyninit.obj $(LIBPLUS)rtlsupt.obj $(LIBTERM)
	-$(RM) kernel.bak
	$(LINK) kernel iprf,kernel,kernel,kernel+$(LIBS);
	-$(RM) kernel.lib

#               *Individual File Dependencies*
kernel.obj: kernel.asm segs.inc

console.obj: console.asm io.inc

printer.obj: printer.asm io.inc

serial.obj: serial.asm io.inc

entry.obj: entry.asm segs.inc $(HDR)stacks.inc

apisupt.obj: apisupt.asm segs.inc

rtlsupt.obj: rtlsupt.asm segs.inc

execrh.obj: execrh.asm segs.inc

int2f.obj: int2f.asm segs.inc

intr.obj: intr.asm segs.inc intr.h

io.obj: io.asm segs.inc

irqstack.obj: irqstack.asm

nls_hc.obj: nls_hc.asm segs.inc

nlssupt.obj: nlssupt.asm segs.inc

procsupt.obj: procsupt.asm segs.inc $(HDR)stacks.inc

dosidle.obj: dosidle.asm segs.inc

# XXX: Special handling for initialization modules -- this is required because
# TC 2.01 cannot handle `#pragma option' like TC 3 can. -- ror4

INITHEADERS=init-mod.h init-dat.h 

CONFIGURATION = turboc.cfg makefile ..\mkfiles\generic.mak ..\mkfiles\$(COMPILER).MAK

HEADERS=\
    $(HDR)portab.h $(HDR)device.h $(HDR)mcb.h $(HDR)pcb.h \
    $(HDR)fat.h $(HDR)fcb.h $(HDR)tail.h $(HDR)time.h $(HDR)process.h \
    $(HDR)dcb.h $(HDR)sft.h $(HDR)cds.h $(HDR)exe.h $(HDR)fnode.h     \
    $(HDR)dirmatch.h $(HDR)file.h $(HDR)clock.h $(HDR)kbd.h $(HDR)error.h  \
    $(HDR)version.h  globals.h proto.h dyndata.h 

# XXX: I generated these using `gcc -MM' and `sed', so they may not be
# completely correct... -- ror4

blockio.obj: blockio.c    $(HEADERS) $(CONFIGURATION)  

break.obj: break.c        $(HEADERS) $(CONFIGURATION)  

chario.obj: chario.c      $(HEADERS) $(CONFIGURATION)  

dosfns.obj: dosfns.c      $(HEADERS) $(CONFIGURATION)  

dosnames.obj: dosnames.c  $(HEADERS) $(CONFIGURATION)  

dsk.obj: dsk.c            $(HEADERS) $(CONFIGURATION)  

error.obj: error.c        $(HEADERS) $(CONFIGURATION)  

fatdir.obj: fatdir.c      $(HEADERS) $(CONFIGURATION)  

fatfs.obj: fatfs.c        $(HEADERS) $(CONFIGURATION)  

fattab.obj: fattab.c      $(HEADERS) $(CONFIGURATION)  

fcbfns.obj: fcbfns.c      $(HEADERS) $(CONFIGURATION)  

inthndlr.obj: inthndlr.c  $(HEADERS) $(CONFIGURATION)  

ioctl.obj: ioctl.c        $(HEADERS) $(CONFIGURATION)  

memmgr.obj: memmgr.c      $(HEADERS) $(CONFIGURATION)  

misc.obj: misc.c          $(HEADERS) $(CONFIGURATION)  

lfnapi.obj: lfnapi.c      $(HEADERS) $(CONFIGURATION)

newstuff.obj: newstuff.c  $(HEADERS) $(CONFIGURATION)  

network.obj: network.c    $(HEADERS) $(CONFIGURATION)  

nls.obj: nls.c            $(HEADERS) $(CONFIGURATION)  

# \
# 001-437.nls

prf.obj: prf.c  $(HDR)\portab.h $(CONFIGURATION)

strings.obj: strings.c  $(CONFIGURATION)

sysclk.obj: sysclk.c    $(HEADERS) $(CONFIGURATION)

syspack.obj: syspack.c  $(HEADERS) $(CONFIGURATION)

systime.obj: systime.c  $(HEADERS) $(CONFIGURATION)

task.obj: task.c        $(HEADERS) $(CONFIGURATION)

# now the funny stuff :-)
# Files in the INIT segment

#   $(CC) $(INITCFLAGS) -c $*.c
#   patchobj $*.obj $(INITPATCH)

config.obj:   config.c  $(INITHEADERS) $(HEADERS) $(CONFIGURATION)  
	$(CC) $(INITCFLAGS) -c $*.c
	$(PATCHOBJ) $*.obj $(INITPATCH)

initoem.obj:  initoem.c  $(INITHEADERS) $(HEADERS) $(CONFIGURATION)  
	$(CC) $(INITCFLAGS) -c $*.c
	$(PATCHOBJ) $*.obj $(INITPATCH)

main.obj:     main.c  $(INITHEADERS) $(HEADERS) $(CONFIGURATION) 
	$(CC) $(INITCFLAGS) -c $*.c
	$(PATCHOBJ) $*.obj $(INITPATCH)

inithma.obj:  inithma.c  $(INITHEADERS) $(HEADERS) $(CONFIGURATION)  
	$(CC) $(INITCFLAGS) -c $*.c
	$(PATCHOBJ) $*.obj $(INITPATCH)

dyninit.obj:  dyninit.c  $(INITHEADERS) $(HEADERS) $(CONFIGURATION)  
	$(CC) $(INITCFLAGS) -c $*.c
	$(PATCHOBJ) $*.obj $(INITPATCH)

initdisk.obj: initdisk.c  $(INITHEADERS) $(HEADERS) $(CONFIGURATION)  
	$(CC) $(INITCFLAGS) -c $*.c
	$(PATCHOBJ) $*.obj $(INITPATCH)

#the printf for INIT_TEXT - yet another special case, this file includes prf.c
iprf.obj: iprf.c prf.c $(HDR)\portab.h $(CONFIGURATION)
	$(CC) $(INITCFLAGS) -c $*.c
	$(PATCHOBJ) $*.obj $(INITPATCH)

#dynamic NEAR data
dyndata.obj: dyndata.c   dyndata.h
	$(CC) $(DYNCFLAGS) -c $*.c
	$(PATCHOBJ) $*.obj $(DYNPATCH)




