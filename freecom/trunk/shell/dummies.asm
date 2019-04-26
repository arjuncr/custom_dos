; $Id: dummies.asm 1822 2018-04-28 18:13:52Z bartoldeman $

; Dummy drivers to be included into FreeCOM itself

%include "../include/model.inc"
%include "../include/stuff.inc"

segment _TEXT
	cglobal dummy_criter_handler
	cglobal end_dummy_criter_handler
dummy_criter_handler:
	mov al, 3			; always fail
	iret
end_dummy_criter_handler:
