; IsMsvcrt90Installed
; Checks target machine for MSVCRT 9.0.21022.8 runtime installed in Windows
; SxS DLL cache.
; Taken from http://www.sneal.net/blog/2007/11/21/NSISCheckForMSVCRT8Runtime.aspx
;
; Returns 1 if found, otherwise 0

Function IsMsvcrt90Installed
	; We can assume version because SxS directory is version specific
	FindFirst $R1 $R0 "$WINDIR\WinSxS\x86_Microsoft.VC90.CRT_1fc8b3b9a1e18e3b_9.0.21022.8*"

	${If} $R0 != ""
		DetailPrint "Visual C++ 2008 (MSVC90) Libraries are already installed"
		StrCpy $R0 1
	${Else}
		DetailPrint "Visual C++ 2008 (MSVC90) Libraries are not installed"
		StrCpy $R0 0
	${EndIf}

	Push $R0
FunctionEnd
