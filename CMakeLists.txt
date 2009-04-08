#Mininum required CMake version
cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6.0)

project(quarkplayer)

#Add options values
include(Options.cmake)

#OWBuild
include(CMakeLists-owbuild.txt)

#Global variables about QuarkPlayer
set(QUARKPLAYER_NAME "QuarkPlayer")
set(QUARKPLAYER_MAJOR_VERSION 0)
set(QUARKPLAYER_MINOR_VERSION 2)
set(QUARKPLAYER_PATCH_VERSION 5)
set(QUARKPLAYER_VERSION
	${QUARKPLAYER_MAJOR_VERSION}.${QUARKPLAYER_MINOR_VERSION}.${QUARKPLAYER_PATCH_VERSION}
)
if (WIN32)
	set(QUARKPLAYER_BINARY_NAME quarkplayer.exe)
endif (WIN32)
set(QUARKPLAYER_DESCRIPTION_SUMMARY "QuarkPlayer, a Phonon media player")
set(QUARKPLAYER_DESCRIPTION
" QuarkPlayer is a multimedia application that plays your music and videos.
 QuarkPlayer can use different backends (DirectShow, MPlayer, Xine,
 GStreamer, VLC...) thanks to Qt and the Phonon library.
 It also relies on an advanced plugin system and is available under Windows,
 Linux and soon Mac OS X."
)
set(QUARKPLAYER_AUTHORS "QuarkPlayer Team")
set(QUARKPLAYER_URL "http://phonon-vlc-mplayer.googlecode.com/")
set(QUARKPLAYER_COPYRIGHT "Copyright (C) 2008-2009  Tanguy Krotoff")
set(QUARKPLAYER_LICENSE_FILE ${CMAKE_SOURCE_DIR}/COPYING)
set(QUARKPLAYER_PACKAGE_FILE quarkplayer-${QUARKPLAYER_VERSION}-rev${SVN_REVISION}-${BUILD_NAME})
set(QUARKPLAYER_SOURCE_PACKAGE_FILE quarkplayer-${QUARKPLAYER_VERSION}-rev${SVN_REVISION}-src)
set(QUARKPLAYER_PACKAGE_AUTHORS ${QUARKPLAYER_AUTHORS})
##


# RPATH handling, see http://www.vtk.org/Wiki/CMake_RPATH_handling
# Make quarkplayer working with the targets install, deb and rpm
set(CMAKE_INSTALL_RPATH .;/usr/lib/quarkplayer)


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

if (MEDIAINFOLIB)
	add_subdirectory(3rdparty/ZenLib)
	add_subdirectory(3rdparty/MediaInfo)
endif (MEDIAINFOLIB)

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
	#include(KDE4Defaults)
	#include(MacroLibrary)

	add_definitions(${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS})
	include_directories(${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
else (KDE4_FOUND)
	set(QT_MIN_VERSION 4.4.0)
	find_package(Qt4 REQUIRED)

	#Includes all wanted definitions for Qt
	include(${QT_USE_FILE})

	#Ok, we can set NEW_TITLE_CHAPTER_HANDLING as we have a patched Phonon library
	set(NEW_TITLE_CHAPTER_HANDLING 1)

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

#Do not use Phonon from Qt or KDE4
#Use Phonon from this repository instead (patched version)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
set(QT_PHONON_LIBRARY phonon)

#set(PHONON_LIBS phonon)
#add_subdirectory(3rdparty/ds9)

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

#Tests
#add_subdirectory(tests)
##

if (WIN32)
	#NSIS installer script
	add_subdirectory(nsis)
endif (WIN32)

if (LINUX)
	#Debian installer script
	add_subdirectory(debian)

	#RPM installer script
	add_subdirectory(rpm)
endif (LINUX)

#CPack instructions
include(CPackConfig.txt)
