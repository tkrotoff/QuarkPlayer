project(libmplayer)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(libmplayer_SRCS
	MyProcess.cpp
	MediaData.cpp
	MediaSettings.cpp
	MPlayerVersion.cpp
	MPlayerProcess.cpp
	MPlayerLoader.cpp
)

qt4_wrap_cpp(libmplayer_SRCS
	MyProcess.h
	MPlayerProcess.h
)

add_library(libmplayer STATIC ${libmplayer_SRCS})

if (KDE4_FOUND)
	target_link_libraries(libmplayer
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${QT_PHONON_LIBRARY}
	)
else (KDE4_FOUND)
	target_link_libraries(libmplayer
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_PHONON_LIBRARY}
	)
endif (KDE4_FOUND)
