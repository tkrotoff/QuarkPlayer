@echo off

echo Setting up Qt 4.6.0 MinGW...
set QTDIR=C:\Qt\2009.05\qt
set PATH=%QTDIR%\bin;%PATH%
set PATH=C:\Qt\2009.05\mingw\bin;%PATH%
set QMAKESPEC=win32-g++

echo Setting up CMake 2.8...
set PATH=C:\Program Files\CMake 2.8\bin;%PATH%