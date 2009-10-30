@echo off

echo Setting up Qt 4.6.0-beta1...
set QTDIR=C:\Qt\4.6.0-beta1
set PATH=%QTDIR%\bin;%PATH%
set QMAKESPEC=win32-msvc2008

echo Setting up CMake 2.6...
rem Remove a substring using string substitution
rem See http://www.dostips.com/DtTipsStringManipulation.php
set PATH=%PATH:C:\Program Files (x86)\CMake 2.6\bin=%
set PATH=C:\Program Files (x86)\CMake 2.6\bin;%PATH%

rem Must be the last line in this script: create a new shell/prompt
echo Setting up Microsoft Visual Studio 2008 (MSVC90)...
call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" amd64
