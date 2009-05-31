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
	PluginManager_win32.cpp
	PluginConfig.cpp
	CommandLineManager.cpp
	CommandLineParser.cpp
	Languages.cpp
	WinDefaultApplication.cpp
	config/Config.cpp
)

qt4_wrap_cpp(quarkplayercore_SRCS
	QuarkPlayer.h
	PluginManager.h
	CommandLineManager.h
	CommandLineParser.h
	config/Config.h
)

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
