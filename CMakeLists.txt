#Mininum required CMake version
cmake_minimum_required(VERSION 2.6)

project(quarkplayer)

set(QUARKPLAYER_MAJOR_VERSION 0)
set(QUARKPLAYER_MINOR_VERSION 2)
set(QUARKPLAYER_PATCH_VERSION 4)

#Add options values
include(Options.cmake)

#OWBuild
include(CMakeLists-owbuild.txt)


#CMake declarations order matters!


#Default installation path
#and other platform specific installation instructions
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
##


#3rdparty libraries with no Qt dependency
if (ZLIB)
	add_subdirectory(3rdparty/zlib)
	set(ZLIB_FOUND 1)
	set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib)
	set(ZLIB_LIBRARY zlib)
endif (ZLIB)

#minizip
add_subdirectory(3rdparty/zlib/contrib/minizip)
set(MINIZIP_FOUND 1)
set(MINIZIP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib/contrib/minizip)
set(MINIZIP_LIBRARY minizip)
##

if (TAGLIB)
	add_subdirectory(3rdparty/taglib)
endif (TAGLIB)
##


#Qt/KDE libraries detection and initialization
if (NOT ENV{KDEDIRS})
	#Ubuntu 8.04, if GNOME is runnng instead of KDE, KDEDIRS variable is not defined
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
reduce_qt_include_dirs()
##


#3rdparty libraries with Qt dependency
add_subdirectory(3rdparty/quazip)
##


#3rdparty libraries with Qt and Phonon dependency
add_subdirectory(3rdparty/phonon)
set(PHONON_LIBS phonon)

#Do not use Phonon from Qt or KDE4
#Use Phonon from this repository instead (patched version)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
set(QT_PHONON_LIBRARY phonon)

add_subdirectory(3rdparty/ds9)
add_subdirectory(phonon-mplayer)
##


#QuarkPlayer
clear_qt_translations()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)
add_subdirectory(libs)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(quarkplayer)
add_subdirectory(quarkplayer/style)
add_subdirectory(quarkplayer-plugins)
add_subdirectory(quarkplayer-app)
##


#CPack instructions
include(CPackConfig.txt)
