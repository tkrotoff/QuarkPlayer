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

	#Phonon detection, this should be integrated into FindQt4.cmake
	FIND_LIBRARY(QT_PHONON_LIBRARY_RELEASE NAMES phonon phonon4 PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
	FIND_LIBRARY(QT_PHONON_LIBRARY_DEBUG NAMES phonon_debug phonond4 PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
	_QT4_ADJUST_LIB_VARS(PHONON)
	##

	#Qt include dir
	include_directories(${QT_INCLUDE_DIR})

	message(STATUS "KDE4 support disabled")
endif (KDE4_FOUND)

add_subdirectory(phonon-vlc-mplayer)
add_subdirectory(tests)
