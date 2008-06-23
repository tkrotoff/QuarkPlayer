project(quarkplayer)

set(quarkplayer_SRCS
	../main.cpp
)

qt4_add_resources(quarkplayer_SRCS
	../quarkplayer.qrc
	../quarkplayer_oxygen.qrc
	../quarkplayer_silk.qrc
	../quarkplayer_tango.qrc
)

if (WIN32)
	list(APPEND quarkplayer_SRCS
		../quarkplayer.rc
	)
endif (WIN32)

set(quarkplayer_TRANSLATIONS
	${quarkplayer_SRCS}
	../translations/quarkplayer_en.ts
	../translations/quarkplayer_fr.ts
	../translations/quarkplayer_de.ts
)

#Pretty slow :/
#qt4_create_translation(quarkplayer_QMS ${quarkplayer_TRANSLATIONS})
set(quarkplayer_QMS "")
##

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_executable(quarkplayer ${quarkplayer_SRCS} ${quarkplayer_QMS})
else (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_executable(quarkplayer WIN32 ${quarkplayer_SRCS} ${quarkplayer_QMS})
endif (CMAKE_BUILD_TYPE STREQUAL Debug)

if (KDE4_FOUND)
	target_link_libraries(quarkplayer
		libquarkplayer
		tkutil
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
		${QT_PHONON_LIBRARY}
	)
else (KDE4_FOUND)
	target_link_libraries(quarkplayer
		libquarkplayer
		tkutil
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_PHONON_LIBRARY}
		${QT_QTMAIN_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS quarkplayer DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${quarkplayer_QMS} DESTINATION ${BIN_INSTALL_DIR}/translations)
