;   $Id: dmy_cbrk.asm 90 2000-12-17 22:02:25Z skaus $
;
;	Dummy ^Break signal catcher -- module
;	Simply ignores any ^Break signals
;	Full relocable
;
;   $Log$
;   Revision 1.1.2.1  2000/12/17 21:57:36  skaus
;   intermediate update 1
;

	clc			;; tell DOS to proceed
	retf 2		;; no IRET!
