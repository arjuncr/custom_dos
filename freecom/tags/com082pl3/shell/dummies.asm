; $Id: dummies.asm 364 2002-04-02 18:13:06Z skaus $

; Dummy drivers to be included into FreeCOM itself

%include "../include/model.inc"

segment _TEXT
	GLOBAL _dummy_criter_handler
	GLOBAL _end_dummy_criter_handler
_dummy_criter_handler:
	mov al, 3			; always fail
	iret
_end_dummy_criter_handler:
