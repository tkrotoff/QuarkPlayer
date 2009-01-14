/**
 * Global variables, already defined.
 *
 * If you add a global variable, declare it here as commented.
 */
;!define BUILD_DIR "..\build\win32-msvc80-release\"
;!define PRODUCT_NAME "QuarkPlayer"
;!define PRODUCT_VERSION "0.2.5"
;!define INSTALLER_NAME "quarkplayer-0.2.4-rev629-win32-msvc90-minsizerel.exe"
;!define BINARY_NAME "quarkplayer.exe"
;!define PRODUCT_DESCRIPTION "QuarkPlayer, a Phonon media player"
;!define PRODUCT_PUBLISHER "Tanguy Krotoff <tkrotoff@gmail.com>"
;!define PRODUCT_URL "http://phonon-vlc-mplayer.googlecode.com/"
;!define PRODUCT_COPYRIGHT "Copyright (C) 2008-2009 Tanguy Krotoff"
;!define PRODUCT_LICENSE_FILE "../COPYING"

!define PRODUCT_REGKEY "Software\${PRODUCT_NAME}"
!define PRODUCT_UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

; Version information for the installer executable
VIAddVersionKey ProductName "${PRODUCT_NAME}"
VIAddVersionKey ProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey Comments "${PRODUCT_DESCRIPTION}"
VIAddVersionKey CompanyName "${PRODUCT_PUBLISHER}"
VIAddVersionKey LegalCopyright "${PRODUCT_COPYRIGHT}"
VIAddVersionKey FileDescription "${PRODUCT_NAME}"
VIAddVersionKey FileVersion "${PRODUCT_VERSION}"
VIAddVersionKey InternalName "${PRODUCT_NAME}"
VIAddVersionKey OriginalFilename "${INSTALLER_NAME}"
VIProductVersion "${PRODUCT_VERSION}.0"

SetCompressor lzma

; Modern UI 1.67 compatible
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING

; Language Selection Dialog Settings
; Remember the installer language
!define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_REGKEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${PRODUCT_LICENSE_FILE}"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "$INSTDIR\${BINARY_NAME}"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; Language files, first language is the default language
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end


Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"

/** Installer name (e.g. quarkplayer-0.2.4-rev629-win32-msvc90-minsizerel.exe). */
OutFile "${INSTALLER_NAME}"

;!include "nsProcess.nsh"

!include "IsUserAdmin.nsi"
Function .onInit
	/** Kills running quarkplayer.exe */
	;${nsProcess::KillProcess} "${BINARY_NAME}" $R0

	!insertmacro MUI_LANGDLL_DISPLAY

	Call IsUserAdmin
	Pop $R0
	StrCmp $R0 "true" isAdmin
		; Not an admin
		StrCpy $INSTDIR "$DOCUMENTS\${PRODUCT_NAME}"
		goto initDone
	isAdmin:
		; User is admin
		StrCpy $INSTDIR "$PROGRAMFILES\${PRODUCT_NAME}"
	initDone:
FunctionEnd

; Gets installation folder from registry if available
InstallDirRegKey HKCU "${PRODUCT_REGKEY}" ""

ShowInstDetails show
ShowUnInstDetails show

Section BaseSection
	!include "files_install.nsi"
SectionEnd

Section -AdditionalIcons
	WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_URL}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section -Post
	; Stores installation folder
	WriteRegStr HKCU "${PRODUCT_REGKEY}" "" $INSTDIR

	; Creates uninstaller
	WriteUninstaller "$INSTDIR\uninstall.exe"

	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "DisplayName" "$(^Name)"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "DisplayIcon" "$INSTDIR\${BINARY_NAME}"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "URLInfoAbout" "${PRODUCT_URL}"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onInit
	!insertmacro MUI_UNGETLANGUAGE
FunctionEnd

Section Uninstall
	!include "files_uninstall.nsi"
SectionEnd
