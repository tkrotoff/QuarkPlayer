@echo off

echo Setting up Qt 4.5.0 MinGW...
set QTDIR=C:\Qt\2009.01\qt
set PATH=%QTDIR%\bin;%PATH%
set PATH=%QTDIR%\..\bin;%QTDIR%\..\mingw\bin;%PATH%
set PATH=%PATH%;%SystemRoot%\System32
set QMAKESPEC=win32-g++

echo Setting up CMake 2.6...
set PATH=C:\Program Files\CMake 2.6\bin;%PATH%
