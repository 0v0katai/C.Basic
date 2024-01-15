@echo off
rem Do not edit! This batch file is created by CASIO fx-9860G SDK.

if exist debug\*.obj  del debug\*.obj
if exist CBASIC.G1A  del CBASIC.G1A

cd debug
if exist FXADDINror.bin  del FXADDINror.bin
"C:\Program Files\CASIO\fx-9860G SDK\OS\SH\Bin\Hmake.exe" Addin.mak
cd ..
if not exist debug\FXADDINror.bin  goto error

"C:\Program Files\CASIO\fx-9860G SDK\Tools\MakeAddinHeader363.exe" "J:\pokecom\9860G\@sdk_program\CBASIC177beta"
if not exist CBASIC.G1A  goto error
echo Build has completed.
goto end

:error
echo Build was not successful.

:end
pause
