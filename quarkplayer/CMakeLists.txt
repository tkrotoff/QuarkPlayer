project(quarkplayercore)

# Cannot put it inside ${CMAKE_CURRENT_BINARY_DIR} this it
# is used by external components (i.e plugins) and the API must
# be coherent
configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
configure_file(version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

add_definitions(-DBUILD_QUARKPLAYER -DQT_QTSINGLEAPPLICATION_IMPORT)

set(quarkplayercore_SRCS
	${CMAKE_CURRENT_BINARY_DIR}/version.cpp
	QuarkPlayer.cpp
	PluginInterface.cpp
	PluginData.cpp
	PluginManager.cpp
	PluginConfig.cpp
	CommandLineManager.cpp
	CommandLineParser.cpp
	Languages.cpp
	WinDefaultApplication.cpp
	config/Config.cpp
	config/ConfigWindow.cpp
	config/GeneralConfigWidget.cpp
	config/SettingsBrowser.cpp
	config/BackendCapabilitiesWidget.cpp
	config/PluginConfigWidget.cpp
	config/WinFileAssociationsConfigWidget.cpp
	config/PlaylistConfig.cpp
	config/ShortcutsConfigWidget.cpp
	config/ShortcutsFileParser.cpp
	config/ShortcutsConfig.cpp
)

qt4_wrap_cpp(quarkplayercore_SRCS
	QuarkPlayer.h
	PluginManager.h
	CommandLineManager.h
	CommandLineParser.h
	config/Config.h
	config/ConfigWindow.h
	config/IConfigWidget.h
	config/GeneralConfigWidget.h
	config/SettingsBrowser.h
	config/BackendCapabilitiesWidget.h
	config/PluginConfigWidget.h
	config/WinFileAssociationsConfigWidget.h
	config/PlaylistConfig.h
	config/ShortcutsConfigWidget.h
	config/ShortcutsConfig.h
)

qt4_wrap_ui(quarkplayercore_SRCS
	config/ConfigWindow.ui
	config/GeneralConfigWidget.ui
	config/SettingsBrowser.ui
	config/BackendCapabilitiesWidget.ui
	config/PluginConfigWidget.ui
	config/WinFileAssociationsConfigWidget.ui
	config/ShortcutsConfigWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(quarkplayercore SHARED ${quarkplayercore_SRCS})

target_link_libraries(quarkplayercore
	tkutil
	filetypes
	qtsingleapplication
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

# Translations
add_qt_translations(${quarkplayercore_SRCS})

install(TARGETS quarkplayercore ${INSTALL_TARGETS_DEFAULT_ARGS})
