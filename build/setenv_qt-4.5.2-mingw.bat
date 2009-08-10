@echo off

echo Setting up Qt 4.5.2 MinGW...
set QTDIR=C:\Qt\4.5.2-mingw
set PATH=%QTDIR%\bin;%PATH%
set PATH=C:\MinGW\bin;%PATH%
set QMAKESPEC=win32-g++

echo Setting up CMake 2.6...
set PATH=C:\Program Files\CMake 2.6\bin;%PATH%
