project(playlistparser)

add_definitions(-DBUILD_PLAYLISTPARSER)

set(playlistparser_SRCS
	PlaylistParser.cpp
	M3UParser.cpp
)

add_library(playlistparser SHARED ${playlistparser_SRCS})

if (KDE4_FOUND)
	target_link_libraries(playlistparser
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(playlistparser
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS playlistparser DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
