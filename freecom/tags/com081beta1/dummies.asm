; $Id: dummies.asm 128 2001-02-18 17:59:37Z skaus $

; Dummy drivers to be included into FreeCOM itself

%include "model.inc"

segment _TEXT
	GLOBAL _dummy_criter_handler
_dummy_criter_handler:
	mov al, 3			; always fail
	iret
