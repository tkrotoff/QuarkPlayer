project(quarkplayer)

set(QUARKPLAYER_MAJOR_VERSION 0)
set(QUARKPLAYER_MINOR_VERSION 1)
set(QUARKPLAYER_PATCH_VERSION 0)

configure_file(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

#For tkutil
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
##

set(quarkplayer_SRCS
	main.cpp
	MainWindow.cpp
	PlayToolBar.cpp
	VideoWidget.cpp
	StatusBar.cpp
	MediaController.cpp
	QuickSettingsWindow.cpp
	AboutWindow.cpp
	MediaDataWidget.cpp
	config/Config.cpp
	config/ConfigWindow.cpp
	config/GeneralConfigWidget.cpp
	config/SettingsBrowser.cpp
)

qt4_add_resources(quarkplayer_SRCS
	quarkplayer.qrc
	quarkplayer_oxygen.qrc
	quarkplayer_silk.qrc
	quarkplayer_tango.qrc
)

qt4_wrap_cpp(quarkplayer_SRCS
	MainWindow.h
	PlayToolBar.h
	VideoWidget.h
	StatusBar.h
	MediaController.h
	QuickSettingsWindow.h
	MediaDataWidget.h
	config/Config.h
	config/ConfigWindow.h
	config/GeneralConfigWidget.h
	config/SettingsBrowser.h
)

if (WIN32)
	list(APPEND quarkplayer_SRCS
		quarkplayer.rc
	)
endif (WIN32)

qt4_wrap_ui(quarkplayer_SRCS
	QuickSettingsWindow.ui
	BackgroundLogoWidget.ui
	AboutWindow.ui
	config/ConfigWindow.ui
	config/GeneralConfigWidget.ui
	config/SettingsBrowser.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(quarkplayer_TRANSLATIONS
	${quarkplayer_SRCS}
	translations/quarkplayer_fr.ts
)
qt4_create_translation(quarkplayer_QMS ${quarkplayer_TRANSLATIONS})
#qt4_add_translation(quarkplayer_QMS ${quarkplayer_TRANSLATIONS})

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_executable(quarkplayer ${quarkplayer_SRCS} ${quarkplayer_QMS})
else (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_executable(quarkplayer WIN32 ${quarkplayer_SRCS} ${quarkplayer_QMS})
endif (CMAKE_BUILD_TYPE STREQUAL Debug)

if (KDE4_FOUND)
	target_link_libraries(quarkplayer
		tkutil
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
		${QT_PHONON_LIBRARY}
	)
else (KDE4_FOUND)
	target_link_libraries(quarkplayer
		tkutil
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_PHONON_LIBRARY}
		${QT_QTMAIN_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS quarkplayer DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${quarkplayer_QMS} DESTINATION ${BIN_INSTALL_DIR}/translations)

add_subdirectory(style)
add_subdirectory(tkutil)
