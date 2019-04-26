#
# makefile for device.lib
#
# $Id: makefile 484 2002-11-18 23:02:07Z bartoldeman $
#


!include "..\mkfiles\generic.mak"


# MICROSOFT C
# -----------
#MODEL  = s
#CFLAGS = /c /Gs /A$(MODEL)
#AFLAGS = /Mx /Dmem$(MODEL)=1
#TERM   = ;

# BORLAND C
# -----------
#MODEL  = s
#CFLAGS = -c -m$(MODEL)
#AFLAGS = /Mx /Dmem$(MODEL)=1
#LIBFLAGS = /c

OBJS   = floppy.obj getvec.obj rdpcclk.obj rdatclk.obj \
wrpcclk.obj wratclk.obj

LIBOBJS= +floppy +getvec +rdpcclk +rdatclk +wrpcclk +wratclk



# Build the LIBRARY
# -----------------
all:            production

production:     ..\lib\device.lib

..\lib\device.lib: device.lib
	copy device.lib ..\lib

clobber:        clean
	-$(RM) device.lib status.me ..\lib\device.lib

clean:
	-$(RM) *.obj *.bak *.crf *.xrf *.map *.lst *.cod *.err

device.lib : $(OBJS)
	-$(RM) device.lib
	$(LIBUTIL) $(LIBFLAGS) device $(LIBOBJS) $(LIBTERM)

