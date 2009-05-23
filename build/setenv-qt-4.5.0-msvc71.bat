@echo off

echo Setting up Qt 4.5.0...
set QTDIR=C:\Qt\4.5.0-msvc71
set PATH=%QTDIR%\bin;%PATH%
set QMAKESPEC=win32-msvc2003

echo Setting up CMake 2.6...
set PATH=C:\Program Files\CMake 2.6\bin;%PATH%

rem Must be the last line in this script: create a new shell/prompt
echo Setting up Microsoft Visual Studio .NET 2003 (MSVC71)...
call "C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"
