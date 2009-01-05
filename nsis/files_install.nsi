Delete "$INSTDIR\*.exe"
Delete "$INSTDIR\*.dll"
Delete "$INSTDIR\*.manifest"
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
CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${BINARY_NAME}"
CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${BINARY_NAME}"

File "${BUILD_DIR}\*.dll"
File "${BUILD_DIR}\*.manifest"

File "${BUILD_DIR}\README"
File "${BUILD_DIR}\ChangeLog"
File "${BUILD_DIR}\COPYING"
File "${BUILD_DIR}\COPYING.LESSER"
File "${BUILD_DIR}\AUTHORS"
File "${BUILD_DIR}\THANKS"

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

/** Allow us to execute quarkplayer.exe */
SetOutPath "$INSTDIR"
