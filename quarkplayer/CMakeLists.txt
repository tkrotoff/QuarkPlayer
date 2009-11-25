project(quarkplayercore)

# Cannot put it inside ${CMAKE_CURRENT_BINARY_DIR} this it
# is used by external components (i.e plugins) and the API must
# be coherent
configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
configure_file(version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

# Counts the lines of code (LOC)
set(QUARKPLAYER_CORE_LOC 0)
set(QUARKPLAYER_PLUGINS_LOC 0)
set(QUARKPLAYER_LIBS_LOC 0)
set(QUARKPLAYER_3RDPARTY_LOC 0)
if (NOT CMAKE_BUILD_TYPE STREQUAL Debug)
	# Takes a lot of time :/
	loc_counter(${CMAKE_SOURCE_DIR}/quarkplayer QUARKPLAYER_CORE_LOC 1)
	loc_counter(${CMAKE_SOURCE_DIR}/quarkplayer-plugins QUARKPLAYER_PLUGINS_LOC 1)
	loc_counter(${CMAKE_SOURCE_DIR}/libs QUARKPLAYER_LIBS_LOC 1)
	loc_counter(${CMAKE_SOURCE_DIR}/3rdparty QUARKPLAYER_3RDPARTY_LOC 1)
endif (NOT CMAKE_BUILD_TYPE STREQUAL Debug)
configure_file(metrics.h.in ${CMAKE_CURRENT_SOURCE_DIR}/metrics.h)
##

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

	QtSingleApplication

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
