Delete "$INSTDIR\icons\mimetypes\*.ico"
RMDir "$INSTDIR\icons\mimetypes\"
RMDir "$INSTDIR\icons\"

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

Delete "$INSTDIR\*"
RMDir "$INSTDIR"
