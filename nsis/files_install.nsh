Delete "$INSTDIR\*"
Delete "$INSTDIR\README"
Delete "$INSTDIR\ChangeLog"
Delete "$INSTDIR\COPYING"
Delete "$INSTDIR\COPYING.LESSER"
Delete "$INSTDIR\AUTHORS"
Delete "$INSTDIR\THANKS"
Delete "$INSTDIR\imageformats\*.dll"
RMDir /r "$INSTDIR\mplayer\"
Delete "$INSTDIR\phonon_backend\*.dll"
Delete "$INSTDIR\plugins\*.dll"
Delete "$INSTDIR\styles\*.dll"
Delete "$INSTDIR\translations\*.qm"


SetOutPath "$INSTDIR"
File "${BUILD_DIR}\*.exe"
File "${BUILD_DIR}\*.dll"

File "${BUILD_DIR}\README"
File "${BUILD_DIR}\ChangeLog"
File "${BUILD_DIR}\COPYING"
File "${BUILD_DIR}\COPYING.LESSER"
File "${BUILD_DIR}\AUTHORS"
File "${BUILD_DIR}\THANKS"

; Detects MSVC71 (Visual C++ 2003)
${If} ${COMPILER_NAME} == "MSVC71"
	;SetOutPath "$INSTDIR"
	;File /nonfatal "C:\WINDOWS\System32\msvcr71.dll"
	;File /nonfatal "C:\WINDOWS\System32\msvcp71.dll"
${EndIf}

; Detects MSVC80 (Visual C++ 2005)
${If} ${COMPILER_NAME} == "MSVC80"
	; Compiler is MSVC80

	Call IsMsvcrt80Installed
	Pop $R0
	${If} $R0 == 0
		;SetOutPath $PLUGINSDIR
		;File /nonfatal "C:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\BootStrapper\Packages\vcredist_x86\vcredist_x86.exe"
		;DetailPrint "Installing Visual C++ 2005 (MSVC80) Libraries"
		;ExecWait '"$PLUGINSDIR\vcredist_x86.exe" /q:a /c:"msiexec /i vcredist.msi /quiet"'
	${EndIf}
${EndIf}

; Detects MSVC90 (Visual C++ 2008)
${If} ${COMPILER_NAME} == "MSVC90"
	; Compiler is MSVC90

	Call IsMsvcrt90Installed
	Pop $R0
	${If} $R0 == 0
		SetOutPath $PLUGINSDIR
		File /nonfatal "C:\Program Files\Microsoft SDKs\Windows\v6.0A\Bootstrapper\Packages\vcredist_x86\vcredist_x86.exe"
		DetailPrint "Installing Visual C++ 2008 (MSVC90) Libraries"
		ExecWait '"$PLUGINSDIR\vcredist_x86.exe" /q:a /c:"msiexec /i vcredist.msi /quiet"'
	${EndIf}
${EndIf}

SetOutPath "$INSTDIR\imageformats\"
File "${BUILD_DIR}\imageformats\*.dll"

SetOutPath "$INSTDIR\mplayer\"
File "${BUILD_DIR}\mplayer\*"

SetOutPath "$INSTDIR\phonon_backend\"
File "${BUILD_DIR}\phonon_backend\*.dll"

SetOutPath "$INSTDIR\plugins\"
File "${BUILD_DIR}\plugins\*.dll"

SetOutPath "$INSTDIR\styles\"
File "${BUILD_DIR}\styles\*.dll"

SetOutPath "$INSTDIR\translations\"
File "${BUILD_DIR}\translations\*.qm"

; Allow to execute quarkplayer.exe
SetOutPath "$INSTDIR"
