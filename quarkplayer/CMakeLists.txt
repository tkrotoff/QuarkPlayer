project(quarkplayercore)

# Cannot put it inside ${CMAKE_CURRENT_BINARY_DIR} this it
# is used by external components (i.e plugins) and the API must
# be coherent
configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
configure_file(version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

add_definitions(-DBUILD_QUARKPLAYER)

# Otherwise QString::fromStdWString() is not defined
# This happens when compiling using the KDE4 libraries
remove_definitions(-DQT_NO_STL)

# Takes a lot of time :/
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
	${CMAKE_CURRENT_BINARY_DIR}/version.cpp
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
	WinDefaultApplication.cpp
	config/Config.cpp
	config/ConfigWindow.cpp
	config/GeneralConfigWidget.cpp
	config/SettingsBrowser.cpp
	config/BackendCapabilitiesWidget.cpp
	config/PluginsConfigWidget.cpp
	config/WinFileAssociationsConfigWidget.cpp
	config/PlaylistConfig.cpp
	config/ShortcutsConfigWidget.cpp
	config/ShortcutsFileParser.cpp
	config/ShortcutsConfig.cpp
)

if (MSVC)
	# Do not treat wchar_t as built-in type otherwise QString::fromStdWString()
	# and QString::toStdWString() get unresolved
	set_source_files_properties(AboutWindow.cpp
		PROPERTIES COMPILE_FLAGS "/Zc:wchar_t-"
	)
endif (MSVC)

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
	config/ShortcutsConfigWidget.h
	config/ShortcutsConfig.h
)

qt4_wrap_ui(quarkplayercore_SRCS
	AboutWindow.ui
	config/ConfigWindow.ui
	config/GeneralConfigWidget.ui
	config/SettingsBrowser.ui
	config/BackendCapabilitiesWidget.ui
	config/PluginsConfigWidget.ui
	config/WinFileAssociationsConfigWidget.ui
	config/ShortcutsConfigWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(quarkplayercore SHARED ${quarkplayercore_SRCS})

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

if (UNIX)
	# Otherwise dlopen, dlsym, dlclose are unresolved
	# This happens when compiling using the KDE4 libraries
	target_link_libraries(quarkplayercore dl)
endif (UNIX)

if (MEDIAINFOLIB)
	target_link_libraries(quarkplayercore mediainfo)
endif (MEDIAINFOLIB)

# Translations
add_qt_translations(${quarkplayercore_SRCS})

install(TARGETS quarkplayercore DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
