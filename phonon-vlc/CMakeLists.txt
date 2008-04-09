project(phonon-vlc-mplayer)

find_package(KDE4)
if (KDE4_FOUND)
	include(KDE4Defaults)
	include(MacroLibrary)

	message(STATUS "KDE4 Support: ${KDE4_FOUND}")
	message(STATUS "KDE4 Dir: ${KDE4_DATA_DIR}")
else (KDE4_FOUND)
	find_package(Qt4 REQUIRED)
endif (KDE4_FOUND)

#Qt include dir
include_directories(${QT_INCLUDE_DIR})

if (KDE4_FOUND)
	add_definitions(${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS} -DKDE4_FOUND)
	include_directories(${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
endif (KDE4_FOUND)

if (UNIX)
	set(QT_PHONON_LIBRARY_DEBUG ${QT_LIBRARY_DIR}/libphonon.so.4.1.0.debug)
endif (UNIX)

if (WIN32)
	set(QT_PHONON_LIBRARY_DEBUG ${QT_LIBRARY_DIR}/phonond4.lib)
endif (WIN32)

#phonon-common
set(phonon_commom_SRCS
	../Backend.cpp
	../MediaObject.cpp
	../VideoWidget.cpp
	../AudioOutput.cpp
)

set(phonon_commom_MOCS
	../Backend.h
	../MediaObject.h
	../VideoWidget.h
	../AudioOutput.h
)
##

add_subdirectory(vlc)
add_subdirectory(mplayer)
