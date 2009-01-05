/** DLL in debug mode. */

SetOutPath "$INSTDIR"
File "${QTDIR}\bin\QtCored4.dll"
File "${QTDIR}\bin\QtGuid4.dll"
File "${QTDIR}\bin\QtXmld4.dll"
File "${QTDIR}\bin\QtSvgd4.dll"

File "C:\WINDOWS\system32\msvcr71d.dll"
File "C:\WINDOWS\system32\msvcp71d.dll"

SetOutPath "$INSTDIR\imageformats"
File "${QTDIR}\plugins\imageformats\qmngd1.dll"
