project(quarkplayer)

set(quarkplayer_SRCS
	main.cpp
)

qt4_add_resources(quarkplayer_SRCS
	quarkplayer.qrc
	quarkplayer_oxygen.qrc
	quarkplayer_silk.qrc
	quarkplayer_tango.qrc
	quarkplayer_crystal.qrc
	quarkplayer_gartoonredux.qrc
)

if (WIN32)
	configure_file(
		quarkplayer.rc
		${CMAKE_CURRENT_BINARY_DIR}/quarkplayer.rc
	)

	list(APPEND quarkplayer_SRCS
		${CMAKE_CURRENT_BINARY_DIR}/quarkplayer.rc
	)
endif (WIN32)

set(quarkplayer_TRANSLATIONS
	${SRCS_TRANSLATIONS}
	translations/quarkplayer_fr.ts
	translations/quarkplayer_de.ts
	translations/quarkplayer_es.ts
)

#Pretty slow :/
set(quarkplayer_QMS "")
qt4_create_translation(quarkplayer_QMS ${quarkplayer_TRANSLATIONS})
##

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_executable(quarkplayer ${quarkplayer_SRCS} ${quarkplayer_QMS})
else (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_executable(quarkplayer WIN32 ${quarkplayer_SRCS} ${quarkplayer_QMS})
endif (CMAKE_BUILD_TYPE STREQUAL Debug)

if (KDE4_FOUND)
	target_link_libraries(quarkplayer
		quarkplayercore
		tkutil
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
		${QT_PHONON_LIBRARY}
	)
else (KDE4_FOUND)
	target_link_libraries(quarkplayer
		quarkplayercore
		tkutil
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_PHONON_LIBRARY}
		${QT_QTMAIN_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS quarkplayer DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${quarkplayer_QMS} DESTINATION ${BIN_INSTALL_DIR}/translations)
