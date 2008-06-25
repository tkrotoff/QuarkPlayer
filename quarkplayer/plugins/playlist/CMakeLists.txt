project(playlist)

set(playlist_SRCS
	PlaylistWidget.cpp
)

qt4_wrap_cpp(playlist_SRCS
	PlaylistWidget.h
)

qt4_wrap_ui(playlist_SRCS
	PlaylistWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(playlist MODULE ${playlist_SRCS})

if (KDE4_FOUND)
	target_link_libraries(playlist
		quarkplayercore
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(playlist
		quarkplayercore
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS playlist DESTINATION ${BIN_INSTALL_DIR}/plugins)
