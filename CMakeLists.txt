# Mininum required CMake version
cmake_minimum_required(VERSION 2.8)

# CMake policy
cmake_policy(VERSION 2.8.0)

project(quarkplayer)

# OWBuild
include(CMakeLists-owbuild.txt)

# Add options values
include(Options.cmake)

# Global variables about QuarkPlayer
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
#set(QUARKPLAYER_SVN_REPOSITORY "http://quarkplayer.googlecode.com/svn/trunk/")
set(QUARKPLAYER_DESCRIPTION_SUMMARY "QuarkPlayer, a Phonon media player")
set(QUARKPLAYER_DESCRIPTION
" QuarkPlayer is a multimedia application that plays your music and videos.
 QuarkPlayer can use different backends (DirectShow, MPlayer, Xine,
 GStreamer, VLC...) thanks to Qt and the Phonon library.
 It also relies on an advanced plugin system and is available under Windows,
 Linux and soon Mac OS X."
)
set(QUARKPLAYER_AUTHORS "QuarkPlayer Team")
set(QUARKPLAYER_URL "http://quarkplayer.googlecode.com/")
set(QUARKPLAYER_COPYRIGHT "Copyright (C) 2008-2009  Tanguy Krotoff")
set(QUARKPLAYER_LICENSE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/COPYING)
set(QUARKPLAYER_PACKAGE_FILE quarkplayer-${QUARKPLAYER_VERSION}-rev${SVN_REVISION}-${BUILD_NAME})
set(QUARKPLAYER_SOURCE_PACKAGE_FILE quarkplayer-${QUARKPLAYER_VERSION}-rev${SVN_REVISION}-src)
set(QUARKPLAYER_PACKAGE_AUTHORS ${QUARKPLAYER_AUTHORS})
##

# RPATH handling, see http://www.vtk.org/Wiki/CMake_RPATH_handling
# Make quarkplayer working with the targets install, deb and rpm
set(CMAKE_INSTALL_RPATH .;/usr/lib/quarkplayer)

#svn_export(${QUARKPLAYER_SVN_REPOSITORY} ${CMAKE_BINARY_DIR}/quarkplayer-svn-export)


# CMake declarations order matters!

# Default installation path
# and other platform specific installation instructions
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/${BUILD_NAME})
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


# 3rdparty libraries with no Qt dependency
if (WIN32)
	add_subdirectory(quarkplayer-plugins/wincontextmenu)
endif (WIN32)

add_subdirectory(3rdparty/hmac)
##

# Qt/KDE libraries detection and initialization
if (NOT ENV{KDEDIRS})
	# Ubuntu 8.04, if GNOME is runnng instead of KDE, KDEDIRS variable is not defined
	# so let's define it
	set(ENV{KDEDIRS} /usr/lib/kde4)
endif (NOT ENV{KDEDIRS})

# Disable KDE4 integration since it gives some troubles
# will be changed in the future
# cf http://code.google.com/p/phonon-vlc-mplayer/issues/detail?id=2
#find_package(KDE4)

if (KDE4_FOUND)
	#include(KDE4Defaults)
	#include(MacroLibrary)

	add_definitions(${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS})
	include_directories(${QDBUS_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
else (KDE4_FOUND)
	set(QT_MIN_VERSION 4.4.0)
	find_package(Qt4 REQUIRED)

	# Do not use Phonon from Qt or KDE4
	# Use Phonon from this repository instead (patched version)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
	set(QT_PHONON_LIBRARY phonon)
	set(PHONON_LIBS
		${QT_PHONON_LIBRARY}
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)

	# Ok, we can set NEW_TITLE_CHAPTER_HANDLING as we have a patched Phonon library
	set(NEW_TITLE_CHAPTER_HANDLING 1)

	# Includes all wanted definitions for Qt
	include(${QT_USE_FILE})

	message(STATUS "KDE4 support disabled")
endif (KDE4_FOUND)

message(STATUS "Qt Directory: ${QT_BINARY_DIR}")

# Optimization: remove all Qt modules from include path
reduce_qt_include_dirs()
##

# 3rdparty libraries with zlib dependency
set(ZLIB_FOUND 1)
set(ZLIB_INCLUDE_DIR
	${QT_INCLUDE_DIR}/../src/3rdparty/zlib
	${QT_QTCORE_INCLUDE_DIR}
)
set(ZLIB_LIBRARY ${QT_QTCORE_LIBRARY})

if (MEDIAINFOLIB)
	add_subdirectory(3rdparty/ZenLib)
	add_subdirectory(3rdparty/MediaInfo)
endif (MEDIAINFOLIB)

if (TAGLIB)
	add_subdirectory(3rdparty/taglib)
endif (TAGLIB)
##

# 3rdparty libraries with Qt dependency
add_subdirectory(3rdparty/QtIOCompressor)
add_subdirectory(3rdparty/modeltest)
add_subdirectory(3rdparty/QtSingleApplication)
##

# 3rdparty libraries with Qt and Phonon dependency
add_subdirectory(3rdparty/phonon)
add_subdirectory(phonon-mplayer)
##

# QuarkPlayer
clear_qt_translations()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)
add_subdirectory(libs)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(quarkplayer)
add_subdirectory(quarkplayer/style)
add_subdirectory(quarkplayer-plugins)
add_subdirectory(quarkplayer-app)
##

if (BUILD_TESTS)
	# Tests (includes unit tests)
	# CTest, see http://www.vtk.org/Wiki/CMake_Testing_With_CTest
	enable_testing()
	add_subdirectory(tests)
endif (BUILD_TESTS)

# make doc: custom target in order to generate the documentation using Doxygen
find_package(Doxygen)
if (DOXYGEN_FOUND)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)
endif (DOXYGEN_FOUND)
##

# Packaging (NSIS, RPM, Debian, src.tar.gz)
add_subdirectory(packaging)

# CPack instructions
include(CPackConfig.txt)
