; $Id: xms_crit.asm 1781 2018-01-25 16:16:21Z bartoldeman $
;	Criter and ^Break handler for external programs for XMS Swap
;	variant

%include "../include/model.inc"
segment _TEXT

;; If the include is in here, NASM locks up, suck all CPU
;	global _lowlevel_cbreak_handler
;_lowlevel_cbreak_handler:
;%include "../criter/dmy_cbrk.asm"

%define XMS_SWAP_CRITER
%define NO_RESOURCE_BLOCK

%include "../criter/criter.asm"
