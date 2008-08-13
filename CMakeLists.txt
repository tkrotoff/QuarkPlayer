#Mininum required CMake version
cmake_minimum_required(VERSION 2.6)

project(quarkplayer)

set(QUARKPLAYER_MAJOR_VERSION 0)
set(QUARKPLAYER_MINOR_VERSION 2)
set(QUARKPLAYER_PATCH_VERSION 0)

#Add options values
include(Options.cmake)

include(CMakeLists-owbuild.txt)

if (NOT ENV{KDEDIRS})
	#Ubuntu 8.04, if GNOME is run instead of KDE, KDEDIRS variable is not defined,
	#let's define it
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
	set(NEW_TITLE_CHAPTER_HANDLING 1)

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

#Default paths
set(INCLUDE_INSTALL_DIR include)
set(BIN_INSTALL_DIR ".")
set(LIB_INSTALL_DIR lib)
set(PLUGIN_INSTALL_DIR ${BIN_INSTALL_DIR}/phonon_backend)
set(INSTALL_TARGETS_DEFAULT_ARGS
	RUNTIME DESTINATION ${BIN_INSTALL_DIR}
)

if (WIN32)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/AUTHORS DESTINATION ".")
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README DESTINATION ".")
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/THANKS DESTINATION ".")
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/COPYING DESTINATION ".")
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/COPYING.LESSER DESTINATION ".")

	#Copy Qt toolkit dll
	if (CMAKE_BUILD_TYPE STREQUAL Debug)
		install(FILES ${QT_BINARY_DIR}/QtCored4.dll DESTINATION ${BIN_INSTALL_DIR})
		install(FILES ${QT_BINARY_DIR}/QtGuid4.dll DESTINATION ${BIN_INSTALL_DIR})
		install(FILES ${QT_BINARY_DIR}/QtNetworkd4.dll DESTINATION ${BIN_INSTALL_DIR})
		install(FILES ${QT_BINARY_DIR}/QtOpenGLd4.dll DESTINATION ${BIN_INSTALL_DIR})
		install(FILES ${QT_BINARY_DIR}/../plugins/imageformats/qjpegd4.dll DESTINATION ${BIN_INSTALL_DIR}/imageformats)
		install(FILES ${QT_BINARY_DIR}/../plugins/imageformats/qgifd4.dll DESTINATION ${BIN_INSTALL_DIR}/imageformats)
	else (CMAKE_BUILD_TYPE STREQUAL Debug)
		install(FILES ${QT_BINARY_DIR}/QtCore4.dll DESTINATION ${BIN_INSTALL_DIR})
		install(FILES ${QT_BINARY_DIR}/QtGui4.dll DESTINATION ${BIN_INSTALL_DIR})
		install(FILES ${QT_BINARY_DIR}/QtNetwork4.dll DESTINATION ${BIN_INSTALL_DIR})
		install(FILES ${QT_BINARY_DIR}/QtOpenGL4.dll DESTINATION ${BIN_INSTALL_DIR})
		install(FILES ${QT_BINARY_DIR}/../plugins/imageformats/qjpeg4.dll DESTINATION ${BIN_INSTALL_DIR}/imageformats)
		install(FILES ${QT_BINARY_DIR}/../plugins/imageformats/qgif4.dll DESTINATION ${BIN_INSTALL_DIR}/imageformats)
	endif (CMAKE_BUILD_TYPE STREQUAL Debug)

	#Copy Qt toolkit translations
	install(FILES ${QT_BINARY_DIR}/../translations/qt_fr.qm DESTINATION ${BIN_INSTALL_DIR}/translations)
	install(FILES ${QT_BINARY_DIR}/../translations/qt_de.qm DESTINATION ${BIN_INSTALL_DIR}/translations)
	install(FILES ${QT_BINARY_DIR}/../translations/qt_es.qm DESTINATION ${BIN_INSTALL_DIR}/translations)
	install(FILES ${QT_BINARY_DIR}/../translations/qt_uk.qm DESTINATION ${BIN_INSTALL_DIR}/translations)

	#Copy MPlayer build from SMPlayer
	#Used by MPlayer Phonon backend
	#SMPlayer' MPlayer build can be found at:
	#http://smplayer.sourceforge.net/
	#just install SMPlayer inside C:/Program Files/SMPlayer
	set(MPLAYER_INSTALL_DIR "C:/Program Files/SMPlayer/mplayer")
	if (EXISTS ${MPLAYER_INSTALL_DIR})
		install(DIRECTORY ${MPLAYER_INSTALL_DIR} DESTINATION ${BIN_INSTALL_DIR})
	endif (EXISTS ${MPLAYER_INSTALL_DIR})

	if (MINGW)
		#When compiled with MinGW, we need mingwm10.dll
		install(FILES "C:/mingw/bin/mingwm10.dll" DESTINATION ${BIN_INSTALL_DIR})
	else (MINGW)
		if (MSVC80)
			#No manifest for QuarkPlayer plugins,
			#see http://lists.trolltech.com/qt-interest/2007-09/msg00790.html for good explanations
			set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")
			set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")

			if (CMAKE_BUILD_TYPE STREQUAL Debug)
				#Copy Microsoft Visual C++ 2005 SP1 runtime
				set(MSVC80CRT "C:/WINDOWS/WinSxS/x86_Microsoft.VC80.DebugCRT_1fc8b3b9a1e18e3b_8.0.50727.762_x-ww_5490cd9f")
				if (EXISTS ${MSVC80CRT})
					install(FILES ${MSVC80CRT}/msvcr80d.dll DESTINATION ${BIN_INSTALL_DIR})
					install(FILES ${MSVC80CRT}/msvcp80d.dll DESTINATION ${BIN_INSTALL_DIR})
				endif (EXISTS ${MSVC80CRT})

				#Copy Microsoft Visual C++ 2005 SP1 manifest
				install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/msvc8/Microsoft.VC80.DebugCRT.manifest DESTINATION ${BIN_INSTALL_DIR})
			else (CMAKE_BUILD_TYPE STREQUAL Debug)
				#Copy Microsoft Visual C++ 2005 SP1 runtime
				set(MSVC80CRT "C:/WINDOWS/WinSxS/x86_Microsoft.VC80.CRT_1fc8b3b9a1e18e3b_8.0.50727.762_x-ww_6b128700")
				if (EXISTS ${MSVC80CRT})
					install(FILES ${MSVC80CRT}/msvcr80.dll DESTINATION ${BIN_INSTALL_DIR})
					install(FILES ${MSVC80CRT}/msvcp80.dll DESTINATION ${BIN_INSTALL_DIR})
				endif (EXISTS ${MSVC80CRT})

				#Copy Microsoft Visual C++ 2005 SP1 manifest
				install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/msvc8/Microsoft.VC80.CRT.manifest DESTINATION ${BIN_INSTALL_DIR})
			endif (CMAKE_BUILD_TYPE STREQUAL Debug)
		endif (MSVC80)
	endif (MINGW)
endif (WIN32)

clear_qt_translations()

add_subdirectory(phonon)
add_subdirectory(vlc-mplayer)
#add_subdirectory(ds9)
#add_subdirectory(tests)

add_subdirectory(3rdparty)

add_subdirectory(libs)

add_subdirectory(quarkplayer)
add_subdirectory(quarkplayer/style)
add_subdirectory(quarkplayer-plugins)
add_subdirectory(quarkplayer-app)

include(CPackConfig.txt)
