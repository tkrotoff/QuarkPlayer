'Run Qt Creator within a hidden shell, parameter 0 means SW_HIDE'
CreateObject("WScript.Shell").Run "cmd /K setenv_qt-4.7-msvc10-x64.bat && C:\Qt\qtcreator-2.0.1\bin\qtcreator.exe", 0
