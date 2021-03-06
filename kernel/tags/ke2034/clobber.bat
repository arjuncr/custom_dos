@echo off

:- batch file to clobber everything
:- $Id: clobber.bat 836 2004-03-27 12:13:10Z bartoldeman $

if not exist config.bat echo You must copy CONFIG.B to CONFIG.BAT and edit it to reflect your setup!
if not exist config.bat goto end

call config.bat
call default.bat

cd utils
%MAKE% clobber

cd ..\lib
%MAKE% clobber

cd ..\drivers
%MAKE% clobber

cd ..\boot
%MAKE% clobber

cd ..\sys
%MAKE% clobber

cd ..\kernel
%MAKE% clobber

cd ..\hdr
if exist *.bak del *.bak

cd ..
if exist *.bak del *.bak
if exist status.me del status.me

:end
default.bat clearset
