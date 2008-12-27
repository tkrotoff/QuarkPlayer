project(quarkplayercore)

#Cannot put it inside ${CMAKE_CURRENT_BINARY_DIR} this it
#is used by external components (i.e plugins) and the API must
#be coherent
configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
configure_file(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

add_definitions(-DBUILD_QUARKPLAYER)

#Takes a lot of time :/
set(QUARKPLAYER_CORE_LOC 0)
set(QUARKPLAYER_PLUGINS_LOC 0)
set(QUARKPLAYER_LIBS_LOC 0)
set(QUARKPLAYER_3RDPARTY_LOC 0)
loc_counter(${CMAKE_SOURCE_DIR}/quarkplayer QUARKPLAYER_CORE_LOC 1)
loc_counter(${CMAKE_SOURCE_DIR}/quarkplayer-plugins QUARKPLAYER_PLUGINS_LOC 1)
loc_counter(${CMAKE_SOURCE_DIR}/libs QUARKPLAYER_LIBS_LOC 1)
loc_counter(${CMAKE_SOURCE_DIR}/3rdparty QUARKPLAYER_3RDPARTY_LOC 1)
configure_file(metrics.h.in ${CMAKE_CURRENT_BINARY_DIR}/metrics.h)

set(quarkplayercore_SRCS
	QuarkPlayer.cpp
	PluginInterface.cpp
	PluginData.cpp
	PluginsManager.cpp
	PluginsConfig.cpp
	MainWindow.cpp
	AboutWindow.cpp
	CommandLineHelp.cpp
	CommandLineParser.cpp
	Languages.cpp
	config/Config.cpp
	config/ConfigWindow.cpp
	config/GeneralConfigWidget.cpp
	config/SettingsBrowser.cpp
	config/BackendCapabilitiesWidget.cpp
	config/PluginsConfigWidget.cpp
	config/WinFileAssociationsConfigWidget.cpp
	config/PlaylistConfig.cpp
	config/ShortcutConfigWidget.cpp
	config/ShortcutsFileParser.cpp
)

qt4_wrap_cpp(quarkplayercore_SRCS
	QuarkPlayer.h
	MainWindow.h
	PluginsManager.h
	CommandLineHelp.h
	config/Config.h
	config/ConfigWindow.h
	config/IConfigWidget.h
	config/GeneralConfigWidget.h
	config/SettingsBrowser.h
	config/BackendCapabilitiesWidget.h
	config/PluginsConfigWidget.h
	config/WinFileAssociationsConfigWidget.h
	config/PlaylistConfig.h
	config/ShortcutConfigWidget.h
)

qt4_wrap_ui(quarkplayercore_SRCS
	AboutWindow.ui
	config/ConfigWindow.ui
	config/GeneralConfigWidget.ui
	config/SettingsBrowser.ui
	config/BackendCapabilitiesWidget.ui
	config/PluginsConfigWidget.ui
	config/WinFileAssociationsConfigWidget.ui
	config/ShortcutConfigWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(quarkplayercore SHARED ${quarkplayercore_SRCS})

if (KDE4_FOUND)
	target_link_libraries(quarkplayercore
		tkutil
		winfileassociations
		filetypes
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
		${KDE4_KDEXML_LIBS}
		${QT_PHONON_LIBRARY}
	)
else (KDE4_FOUND)
	target_link_libraries(quarkplayercore
		tkutil
		winfileassociations
		filetypes
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTXML_LIBRARY}
		${QT_PHONON_LIBRARY}
		${QT_QTMAIN_LIBRARY}
	)
endif (KDE4_FOUND)

#Translations
add_qt_translations(${quarkplayercore_SRCS})

install(TARGETS quarkplayercore DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
