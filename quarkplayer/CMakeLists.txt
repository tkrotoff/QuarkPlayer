project(libquarkplayer)

set(QUARKPLAYER_MAJOR_VERSION 0)
set(QUARKPLAYER_MINOR_VERSION 1)
set(QUARKPLAYER_PATCH_VERSION 0)

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
configure_file(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

add_definitions(-DBUILD_QUARKPLAYER)

#Takes a lot of time :/
#loc_counter(${CMAKE_CURRENT_SOURCE_DIR} QUARKPLAYER_ROOT_LOC 0)
#loc_counter(${CMAKE_CURRENT_SOURCE_DIR}/plugins QUARKPLAYER_PLUGINS_LOC 1)
#loc_counter(${CMAKE_CURRENT_SOURCE_DIR}/tkutil QUARKPLAYER_TKUTIL_LOC 1)
#loc_counter(${CMAKE_CURRENT_SOURCE_DIR} QUARKPLAYER_LOC 1)
configure_file(metrics.h.in ${CMAKE_CURRENT_SOURCE_DIR}/metrics.h)

set(libquarkplayer_SRCS
	QuarkPlayer.cpp
	PluginInterface.cpp
	PluginManager.cpp
	MainWindow.cpp
	AboutWindow.cpp
	FileExtensions.cpp
	CommandLineHelp.cpp
	CommandLineParser.cpp
	config/Config.cpp
	config/ConfigWindow.cpp
	config/GeneralConfigWidget.cpp
	config/SettingsBrowser.cpp
	config/BackendCapabilitiesWidget.cpp
)

qt4_wrap_cpp(libquarkplayer_SRCS
	QuarkPlayer.h
	MainWindow.h
	PluginManager.h
	config/Config.h
	config/ConfigWindow.h
	config/IConfigWidget.h
	config/GeneralConfigWidget.h
	config/SettingsBrowser.h
	config/BackendCapabilitiesWidget.h
)

qt4_wrap_ui(libquarkplayer_SRCS
	AboutWindow.ui
	config/ConfigWindow.ui
	config/GeneralConfigWidget.ui
	config/SettingsBrowser.ui
	config/BackendCapabilitiesWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(libquarkplayer SHARED ${libquarkplayer_SRCS})

if (KDE4_FOUND)
	target_link_libraries(libquarkplayer
		tkutil
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
		${QT_PHONON_LIBRARY}
	)
else (KDE4_FOUND)
	target_link_libraries(libquarkplayer
		tkutil
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_PHONON_LIBRARY}
		${QT_QTMAIN_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS libquarkplayer DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
