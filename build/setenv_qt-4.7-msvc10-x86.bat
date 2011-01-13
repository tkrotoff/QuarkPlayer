@echo off

echo Setting up Qt 4.7.1...
set QTDIR=C:\Qt\4.7.1
set PATH=%QTDIR%\bin;%PATH%
set QMAKESPEC=win32-msvc2010

echo Setting up CMake 2.8...
rem Remove a substring using string substitution
rem See http://www.dostips.com/DtTipsStringManipulation.php
set PATH=%PATH:C:\Program Files\CMake 2.8\bin=%
set PATH=C:\Program Files\CMake 2.8\bin;%PATH%

rem Must be the last line in this script: create a new shell/prompt
echo Setting up Microsoft Visual Studio 2010 (MSVC100)...
call "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
