; $Id: dummies.asm 158 2001-03-07 19:49:01Z skaus $

; Dummy drivers to be included into FreeCOM itself

%include "model.inc"

segment _TEXT
	GLOBAL _dummy_criter_handler
_dummy_criter_handler:
	mov al, 3			; always fail
	iret
