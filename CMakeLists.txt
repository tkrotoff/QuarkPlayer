#Mininum required CMake version
cmake_minimum_required(VERSION 2.6)

project(quarkplayer)

set(QUARKPLAYER_MAJOR_VERSION 0)
set(QUARKPLAYER_MINOR_VERSION 2)
set(QUARKPLAYER_PATCH_VERSION 2)

#Add options values
include(Options.cmake)

include(CMakeLists-owbuild.txt)

if (NOT ENV{KDEDIRS})
	#Ubuntu 8.04, if GNOME is run instead of KDE, KDEDIRS variable is not defined
	#so let's define it
	set(ENV{KDEDIRS} /usr/lib/kde4)
endif (NOT ENV{KDEDIRS})

#Desactivate KDE4 integration since it gives some troubles
#will be changed in the future
#cf http://code.google.com/p/phonon-vlc-mplayer/issues/detail?id=2
#find_package(KDE4)

if (KDE4_FOUND)
	include(KDE4Defaults)
	include(MacroLibrary)

	add_definitions(${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS})
	include_directories(${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
else (KDE4_FOUND)
	set(QT_MIN_VERSION 4.4.0)
	find_package(Qt4 REQUIRED)

	#Includes all wanted definitions for Qt
	include(${QT_USE_FILE})

	#Ok, we can set NEW_TITLE_CHAPTER_HANDLING as we have a patched Phonon library
	#set(NEW_TITLE_CHAPTER_HANDLING 1)

	message(STATUS "KDE4 support disabled")
endif (KDE4_FOUND)

#Optimization: remove all Qt modules from include path
#This will reduce compilation time (less include directories == compilation time reduced)
reduce_qt_include_dirs()

#Does not use Phonon from Qt or KDE4
#Uses Phonon from this repository instead
#CMake declarations order matters!!!
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
set(QT_PHONON_LIBRARY phonon)

#Default installation path
set(CMAKE_INSTALL_PREFIX ${BUILD_DIR})

if (WIN32)
	include(CMakeLists-win32.txt)
endif (WIN32)
if (LINUX)
	include(CMakeLists-linux.txt)
endif (LINUX)
if (APPLE)
	include(CMakeLists-macos.txt)
endif (APPLE)

clear_qt_translations()

add_subdirectory(3rdparty)

add_subdirectory(vlc-mplayer)

add_subdirectory(libs)

add_subdirectory(quarkplayer)
add_subdirectory(quarkplayer/style)
add_subdirectory(quarkplayer-plugins)
add_subdirectory(quarkplayer-app)

include(CPackConfig.txt)
