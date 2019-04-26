#
# makefile for libm.lib
#
# $Id: makefile 544 2003-03-15 04:32:37Z bartoldeman $
#


!include "..\mkfiles\generic.mak"


libm.lib:	$(CLIB)
	-$(RM) libm.lib
	$(LIBUTIL) $(CLIB) $(MATH_EXTRACT) $(LIBTERM)
	for %i in (*.obj) do ..\utils\patchobj CODE=LCODE TEXT=_LOWTEXT %i
	$(LIBUTIL) libm $(MATH_INSERT)     $(LIBTERM)
	-$(RM) *.OBJ


clobber:        clean
	-$(RM) libm.lib status.me

clean:
	-$(RM) *.obj *.bak

