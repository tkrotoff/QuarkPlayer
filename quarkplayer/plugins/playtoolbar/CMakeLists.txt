project(playtoolbar)

set(playtoolbar_SRCS
	PlayToolBar.cpp
)

qt4_wrap_cpp(playtoolbar_SRCS
	PlayToolBar.h
)

add_library(playtoolbar SHARED ${playtoolbar_SRCS})

if (KDE4_FOUND)
	target_link_libraries(playtoolbar
		libquarkplayer
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(playtoolbar
		libquarkplayer
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS playtoolbar DESTINATION ${BIN_INSTALL_DIR}/plugins)
