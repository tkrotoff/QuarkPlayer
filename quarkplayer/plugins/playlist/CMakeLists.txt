project(playlist)

set(playlist_SRCS
	PlaylistWidget.cpp
	PlaylistModel.cpp
	Track.cpp
)

qt4_wrap_cpp(playlist_SRCS
	PlaylistWidget.h
	PlaylistModel.h
)

add_library(playlist MODULE ${playlist_SRCS})

if (KDE4_FOUND)
	target_link_libraries(playlist
		quarkplayercore
		tkutil
		playlistparser
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(playlist
		quarkplayercore
		tkutil
		playlistparser
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS playlist DESTINATION ${BIN_INSTALL_DIR}/plugins)
