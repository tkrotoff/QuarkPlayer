@echo off

echo Setting up Qt 4.6.2...
set QTDIR=C:\Qt\4.6.2
set PATH=%QTDIR%\bin;%PATH%
set QMAKESPEC=win32-msvc2008

echo Setting up CMake 2.8...
rem Remove a substring using string substitution
rem See http://www.dostips.com/DtTipsStringManipulation.php
set PATH=%PATH:C:\Program Files\CMake 2.8\bin=%
set PATH=C:\Program Files\CMake 2.8\bin;%PATH%

rem Must be the last line in this script: create a new shell/prompt
echo Setting up Microsoft Visual Studio 2008 (MSVC90)...
call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
