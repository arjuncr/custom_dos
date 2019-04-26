#
# makefile for libm.lib
#
# $Id: makefile 482 2002-11-18 22:51:27Z bartoldeman $
#


!include "..\mkfiles\generic.mak"


libm.lib:	$(CLIB)
	-$(RM) libm.lib
	$(LIBUTIL) $(CLIB) $(MATH_EXTRACT) $(LIBTERM)
	$(LIBUTIL) libm $(MATH_INSERT)     $(LIBTERM)
	-$(RM) *.OBJ


clobber:        clean
	-$(RM) libm.lib status.me

clean:
	-$(RM) *.obj *.bak

