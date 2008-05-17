# Mininum required CMake version
cmake_minimum_required(VERSION 2.6)

project(phonon-vlc-mplayer)

include(CMakeLists-owbuild.txt)

find_package(KDE4)
if (KDE4_FOUND)
	include(KDE4Defaults)
	include(MacroLibrary)

	add_definitions(${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS} -DKDE4_FOUND)
	include_directories(${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

	message(STATUS "KDE4 support enabled")
	message(STATUS "KDE4 dir: ${KDE4_DATA_DIR}")
else (KDE4_FOUND)
	find_package(Qt4 REQUIRED)

	#Includes all wanted definitions for Qt
	include(${QT_USE_FILE})

	#Does not use Phonon from Qt
	#Uses Phonon from this repository instead
	#CMake declarations matters!!!
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
	set(QT_PHONON_LIBRARY phonon)

	#Ok, we can set NEW_TITLE_CHAPTER_HANDLING as we have a patched Phonon library
	add_definitions(-DNEW_TITLE_CHAPTER_HANDLING)

	message(STATUS "KDE4 support disabled")


	#Optimization: remove all Qt modules from include path
	#This will reduce compilation time (less include directories == compilation time reduced)
	reduce_qt_include_dirs()

endif (KDE4_FOUND)

#Default installation path
set(CMAKE_INSTALL_PREFIX ${BUILD_DIR})

#Default paths
set(INCLUDE_INSTALL_DIR include)
set(BIN_INSTALL_DIR bin)
set(LIB_INSTALL_DIR lib)
set(PLUGIN_INSTALL_DIR ${BIN_INSTALL_DIR}/phonon_backend)
set(INSTALL_TARGETS_DEFAULT_ARGS
	RUNTIME DESTINATION ${BIN_INSTALL_DIR}
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Devel
)

if (WIN32)
	if (CMAKE_BUILD_TYPE STREQUAL Debug)
		install(FILES ${QT_BINARY_DIR}/QtCored4.dll DESTINATION ${BIN_INSTALL_DIR})
		install(FILES ${QT_BINARY_DIR}/QtGuid4.dll DESTINATION ${BIN_INSTALL_DIR})
	else (CMAKE_BUILD_TYPE STREQUAL Debug)
		install(FILES ${QT_BINARY_DIR}/QtCore4.dll DESTINATION ${BIN_INSTALL_DIR})
		install(FILES ${QT_BINARY_DIR}/QtGui4.dll DESTINATION ${BIN_INSTALL_DIR})
	endif (CMAKE_BUILD_TYPE STREQUAL Debug)
	if (MINGW)
		install(FILES C:/mingw/bin/mingwm10.dll DESTINATION ${BIN_INSTALL_DIR})
	endif (MINGW)
endif (WIN32)

add_subdirectory(phonon)
add_subdirectory(phonon-vlc-mplayer)
add_subdirectory(tests)

include(CPackConfig.txt)
