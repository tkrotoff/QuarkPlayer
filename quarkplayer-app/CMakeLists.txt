project(quarkplayer)

include_directories(${QTSINGLEAPPLICATION_INCLUDE_DIRS})
add_definitions(${QTSINGLEAPPLICATION_DEFINITIONS})

if (STATICPLUGINS)
	add_definitions(-DSTATICPLUGINS)
endif (STATICPLUGINS)

set(quarkplayer_SRCS
	main.cpp
)

qt4_add_resources(quarkplayer_SRCS
	quarkplayer.qrc
	silk.qrc
	fugue.qrc
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

set(TS_FILES
	translations/quarkplayer_fr.ts
	translations/quarkplayer_de.ts
	translations/quarkplayer_es.ts
	translations/quarkplayer_ru.ts
)

set(QM_FILES "")
if (CMAKE_BUILD_TYPE STREQUAL Debug)
	# Console output under Windows
	add_executable(quarkplayer ${quarkplayer_SRCS})
else (CMAKE_BUILD_TYPE STREQUAL Debug)
	# Pretty slow :/
	qt4_create_translation(
		QM_FILES ${SRCS_TRANSLATIONS} ${TS_FILES} OPTIONS -noobsolete
	)
	##

	# WIN32: no console output under Windows
	add_executable(quarkplayer WIN32 ${quarkplayer_SRCS} ${QM_FILES})
endif (CMAKE_BUILD_TYPE STREQUAL Debug)

target_link_libraries(quarkplayer
	QuarkPlayerCore
	TkUtil

	${QTSINGLEAPPLICATION_LIBRARIES}

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_PHONON_LIBRARY}
	${QT_QTMAIN_LIBRARY}
)

if (STATICPLUGINS)
	# Static plugins
	# Static plugins are faster than dynamic plugins:
	# it takes almost no time to load them
	target_link_libraries(quarkplayer
		MainWindow
		ConfigWindow
		FileBrowser
		FindSubtitles
		MediaController
		Playlist
		PlayToolBar
		QuickSettings
		StatusBar
		VideoWidget
		#OSD
	)
endif (STATICPLUGINS)

install(TARGETS quarkplayer ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES ${QM_FILES} DESTINATION ${BIN_INSTALL_DIR}/translations)
