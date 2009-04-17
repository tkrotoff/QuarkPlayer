@echo off

echo Setting up Qt 4.4.3 MinGW...
set QTDIR=C:\Qt\4.4.3-mingw
set PATH=%QTDIR%\bin;%PATH%
set PATH=%PATH%;C:\MinGW\bin
set PATH=%PATH%;%SystemRoot%\System32
set QMAKESPEC=win32-g++

echo Setting up CMake 2.6...
set PATH=C:\Program Files\CMake 2.6\bin;%PATH%
