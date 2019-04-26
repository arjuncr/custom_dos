#
# makefile for libm.lib
#
# $Id: makefile 600 2003-06-15 19:05:43Z bartoldeman $
#


!include "..\mkfiles\generic.mak"


libm.lib:	$(CLIB)
	-$(RM) libm.lib
	$(LIBUTIL) $(CLIB) $(MATH_EXTRACT) $(LIBTERM)
	$(COMSPEC) /c for %i in (*.obj) do ..\utils\patchobj CODE=LCODE %i
	$(LIBUTIL) libm $(MATH_INSERT)     $(LIBTERM)
	-$(RM) *.OBJ


clobber:        clean
	-$(RM) libm.lib status.me

clean:
	-$(RM) *.obj *.bak

