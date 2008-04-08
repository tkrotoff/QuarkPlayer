project(phonon-mplayer)

find_package(KDE4)
if (KDE4_FOUND)
	include(KDE4Defaults)
	include(MacroLibrary)
else (KDE4_FOUND)
	find_package(Qt4 REQUIRED)
endif (KDE4_FOUND)

message(STATUS "KDE4 Support: ${KDE4_FOUND}")
message(STATUS "KDE4 Dir: ${KDE4_DATA_DIR}")

include_directories(${QT_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (KDE4_FOUND)
	add_definitions(${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS} -DKDE4_FOUND)
	include_directories(${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
endif (KDE4_FOUND)



#Because of libsmplayer includes <QObject> rather than include <QtCore/QObject>
#Increases compilation time :/
include(${QT_USE_FILE})

add_definitions(-DMINILIB)

#libsmplayer
set(libsmplayer_SRCS
	smplayer/core.cpp
	smplayer/desktopinfo.cpp
	smplayer/global.cpp
	smplayer/helper.cpp
	smplayer/mediadata.cpp
	smplayer/mediasettings.cpp
	smplayer/mplayerprocess.cpp
	smplayer/mplayerversion.cpp
	smplayer/mplayerwindow.cpp
	smplayer/myprocess.cpp
	smplayer/preferences.cpp
	smplayer/subtracks.cpp
	smplayer/trackdata.cpp
	smplayer/tracks.cpp
	smplayer/smplayercorelib.cpp
)

if (WIN32)
	set(libsmplayer_SRCS
		${libsmplayer_SRCS}
		smplayer/screensaver.cpp
	)
endif (WIN32)

qt4_wrap_cpp(libsmplayer_SRCS
	smplayer/core.h
	smplayer/mplayerprocess.h
	smplayer/mplayerwindow.h
	smplayer/myprocess.h
	smplayer/smplayercorelib.h
)

add_library(libsmplayer STATIC ${libsmplayer_SRCS})




#phonon_mplayer
include_directories(
	smplayer
)

set(phonon_mplayer_SRCS
	Backend.cpp
	MediaObject.cpp
	VideoWidget.cpp
	AudioOutput.cpp
)

qt4_wrap_cpp(phonon_mplayer_SRCS
	Backend.h
	MediaObject.h
	VideoWidget.h
	AudioOutput.h
)

add_library(phonon_mplayer MODULE ${phonon_mplayer_SRCS})

if (UNIX)
	set(QT_PHONON_LIBRARY_DEBUG ${QT_LIBRARY_DIR}/libphonon.so.4.1.0.debug)
endif (UNIX)

if (WIN32)
	set(QT_PHONON_LIBRARY_DEBUG ${QT_LIBRARY_DIR}/phonond4.lib)
endif (WIN32)

if (KDE4_FOUND)
	target_link_libraries(phonon_mplayer
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_PHONON_LIBS}
		libsmplayer
	)
else (KDE4_FOUND)
	target_link_libraries(phonon_mplayer
		${QT_QTCORE_LIBRARY_DEBUG}
		${QT_QTGUI_LIBRARY_DEBUG}
		${QT_PHONON_LIBRARY_DEBUG}
		libsmplayer
	)
endif (KDE4_FOUND)

if (KDE4_FOUND)
	install(TARGETS phonon_mplayer DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/phonon_backend)
	install(FILES mplayer.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)
	install(FILES icons/16x16/phonon-mplayer.png DESTINATION ${ICON_INSTALL_DIR}/oxygen/16x16/apps)
	install(FILES icons/22x22/phonon-mplayer.png DESTINATION ${ICON_INSTALL_DIR}/oxygen/22x22/apps)
	install(FILES icons/32x32/phonon-mplayer.png DESTINATION ${ICON_INSTALL_DIR}/oxygen/32x32/apps)
	install(FILES icons/48x48/phonon-mplayer.png DESTINATION ${ICON_INSTALL_DIR}/oxygen/48x48/apps)

	#Ubuntu 8.04 Hardy
	install(TARGETS phonon_mplayer DESTINATION /usr/lib/kde4/lib/kde4)
	install(FILES mplayer.desktop DESTINATION /usr/lib/kde4/share/kde4/services/phononbackends)
	install(FILES icons/16x16/phonon-mplayer.png DESTINATION /usr/lib/kde4/share/icons/oxygen/16x16/apps)
	install(FILES icons/22x22/phonon-mplayer.png DESTINATION /usr/lib/kde4/share/icons/oxygen/22x22/apps)
	install(FILES icons/32x32/phonon-mplayer.png DESTINATION /usr/lib/kde4/share/icons/oxygen/32x32/apps)
	install(FILES icons/48x48/phonon-mplayer.png DESTINATION /usr/lib/kde4/share/icons/oxygen/48x48/apps)
else (KDE4_FOUND)
	install(TARGETS phonon_mplayer DESTINATION ${QT_PLUGINS_DIR}/phonon_backend)
endif (KDE4_FOUND)
