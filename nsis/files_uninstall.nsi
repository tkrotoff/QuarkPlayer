/** Kills the running quarkplayer.exe */
;${nsProcess::KillProcess} "${BINARY_NAME}" $R0

Delete "$INSTDIR\imageformats\*.dll"
RMDir "$INSTDIR\imageformats\"

RMDir /r "$INSTDIR\mplayer\"

Delete "$INSTDIR\phonon_backend\*.dll"
RMDir "$INSTDIR\phonon_backend\"

Delete "$INSTDIR\plugins\*.dll"
RMDir "$INSTDIR\plugins\"

Delete "$INSTDIR\styles\*.dll"
RMDir "$INSTDIR\styles\"

Delete "$INSTDIR\translations\*.qm"
RMDir "$INSTDIR\translations\"

Delete "$INSTDIR\*.exe"
Delete "$INSTDIR\*.dll"
Delete "$INSTDIR\*.manifest"
Delete "$INSTDIR\README"
Delete "$INSTDIR\ChangeLog"
Delete "$INSTDIR\COPYING"
Delete "$INSTDIR\COPYING.LESSER"
Delete "$INSTDIR\AUTHORS"
Delete "$INSTDIR\THANKS"
RMDir "$INSTDIR"

Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
Delete "$SMPROGRAMS\${PRODUCT_NAME}\Website.lnk"
Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
RMDir "$SMPROGRAMS\${PRODUCT_NAME}"

/**
 * Removes the configuration file.
 * Only if the user asks for it.
 */
;Delete "${WENGODIR}\contacts\*"
;RMDir "${WENGODIR}\contacts\"

DeleteRegKey HKLM "${PRODUCT_UNINSTALL_KEY}"
DeleteRegKey HKCU "${PRODUCT_REGKEY}"
SetAutoClose true
