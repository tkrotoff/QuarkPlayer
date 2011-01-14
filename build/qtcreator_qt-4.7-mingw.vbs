'Run Qt Creator within a hidden shell, parameter 0 means SW_HIDE'
CreateObject("WScript.Shell").Run "cmd /K setenv_qt-4.7-mingw.bat && C:\Qt\2010.05\bin\qtcreator.exe", 0
