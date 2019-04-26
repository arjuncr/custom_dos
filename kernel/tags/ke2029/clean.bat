@echo off

rem batch file to clean everything

rem $Id: clean.bat 482 2002-11-18 22:51:27Z bartoldeman $

if not exist config.bat goto noconfigbat
goto start

:noconfigbat
echo You must copy CONFIG.B to CONFIG.BAT and edit it to reflect your setup!
goto end

:start
call config.bat
call getmake.bat

cd utils
%MAKE% clean

cd ..\lib
%MAKE% clean

cd ..\drivers
%MAKE% clean

cd ..\boot
%MAKE% clean

cd ..\sys
%MAKE% clean

cd ..\kernel
%MAKE% clean

cd ..\hdr
del *.bak

cd ..

del *.bak

:end
set MAKE=
set COMPILER=
