project(mediacontroller)

set(mediacontroller_SRCS
	MediaController.cpp
	MediaControllerToolBar.cpp
)

qt4_wrap_cpp(mediacontroller_SRCS
	MediaController.h
	MediaControllerToolBar.h
)

add_library(mediacontroller SHARED ${mediacontroller_SRCS})

if (KDE4_FOUND)
	target_link_libraries(mediacontroller
		libquarkplayer
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(mediacontroller
		libquarkplayer
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS mediacontroller DESTINATION ${BIN_INSTALL_DIR}/plugins)
