@echo off

mkdir %USERPROFILE%\Documents\Arduino\libraries\OHoCo

mklink %USERPROFILE%\Documents\Arduino\libraries\OHoCo\keywords.txt %~dp0keywords.txt
mklink %USERPROFILE%\Documents\Arduino\libraries\OHoCo\OHoCo.h %~dp0OHoCo.h
mklink %USERPROFILE%\Documents\Arduino\libraries\OHoCo\OHoCo.cpp %~dp0OHoCo.cpp

echo.
pause