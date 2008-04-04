project(phonon-mplayer)

find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDE_DIR})

#because of libsmplayer include <QObject> rather than include <QtCore/QObject>
include(${QT_USE_FILE})




#libsmplayer
set(libsmplayer_SRCS
	smplayer/core.cpp
	smplayer/desktopinfo.cpp
	smplayer/global.cpp
	smplayer/helper.cpp
	smplayer/images.cpp
	smplayer/mediadata.cpp
	smplayer/mediasettings.cpp
	smplayer/mplayerprocess.cpp
	smplayer/mplayerversion.cpp
	smplayer/mplayerwindow.cpp
	smplayer/myprocess.cpp
	smplayer/preferences.cpp
	smplayer/screensaver.cpp
	smplayer/subtracks.cpp
	smplayer/trackdata.cpp
	smplayer/tracks.cpp
	smplayer/translator.cpp
)

qt4_wrap_cpp(libsmplayer_SRCS
	smplayer/config.h
	smplayer/constants.h
	smplayer/core.h
	smplayer/desktopinfo.h
	smplayer/global.h
	smplayer/helper.h
	smplayer/images.h
	smplayer/mediadata.h
	smplayer/mediasettings.h
	smplayer/mplayerprocess.h
	smplayer/mplayerversion.h
	smplayer/mplayerwindow.h
	smplayer/myprocess.h
	smplayer/preferences.h
	smplayer/screensaver.h
	smplayer/subtracks.h
	smplayer/trackdata.h
	smplayer/tracks.h
	smplayer/translator.h
)

add_library(libsmplayer STATIC ${libsmplayer_SRCS})

target_link_libraries(libsmplayer
	${QT_QTCORE_LIBRARY}
)




#phonon_mplayer
include_directories(
	smplayer
)

set(phonon_mplayer_SRCS
	backend.cpp
	mediaobject.cpp
	videowidget.cpp
	audiooutput.cpp
)

qt4_wrap_cpp(phonon_mplayer_SRCS
	backend.h
	mediaobject.h
	videowidget.h
	audiooutput.h
)

add_library(phonon_mplayer MODULE ${phonon_mplayer_SRCS})

target_link_libraries(phonon_mplayer
	${QT_QTCORE_LIBRARY}
	${QT_LIBRARY_DIR}/phonond4.lib
	libsmplayer.lib
	${QT_QTGUI_LIBRARY}
)

install(TARGETS phonon_mplayer
	RUNTIME DESTINATION "C:/Qt/4.4.0/plugins/phonon_backend"
	LIBRARY DESTINATION "C:/Qt/4.4.0/plugins/phonon_backend"
	ARCHIVE DESTINATION "C:/Qt/4.4.0/plugins/phonon_backend"
)
