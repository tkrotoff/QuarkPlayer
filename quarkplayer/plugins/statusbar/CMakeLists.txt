project(statusbar)

set(statusbar_SRCS
	StatusBar.cpp
)

qt4_wrap_cpp(statusbar_SRCS
	StatusBar.h
)

add_library(statusbar MODULE ${statusbar_SRCS})

if (KDE4_FOUND)
	target_link_libraries(statusbar
		libquarkplayer
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(statusbar
		libquarkplayer
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS statusbar DESTINATION ${BIN_INSTALL_DIR}/plugins)
