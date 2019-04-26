#
# makefile for libm.lib
#
# $Id: makefile 1065 2004-11-18 11:20:04Z luchezar $
#

!include "../mkfiles/generic.mak"

########################################################################

all:		libm.lib

libm.lib:	$(CLIB) $(DEPENDS)
		-$(RM) $*.lib
		$(LIBUTIL) $(CLIB) $(MATH_EXTRACT)$(LIBTERM)
		$(COMSPEC) /c for %i in (*.obj) do ..\utils\patchobj CODE=LCODE %i
		$(LIBUTIL) $* $(MATH_INSERT)$(LIBTERM)
		-$(RM) *.obj

########################################################################

clean:
		-$(RM) *.bak *.cod *.crf *.err *.las *.lst *.map *.obj *.xrf

clobber:	clean
		-$(RM) libm.lib status.me
