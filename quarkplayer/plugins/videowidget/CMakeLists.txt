project(videowidget)

set(videowidget_SRCS
	VideoWidgetPlugin.cpp
	VideoWidget.cpp
	MediaDataWidget.cpp
)

qt4_wrap_cpp(videowidget_SRCS
	VideoWidgetPlugin.h
	VideoWidget.h
	MediaDataWidget.h
)

qt4_wrap_ui(videowidget_SRCS
	BackgroundLogoWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(videowidget SHARED ${videowidget_SRCS})

if (KDE4_FOUND)
	target_link_libraries(videowidget
		libquarkplayer
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(videowidget
		libquarkplayer
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS videowidget DESTINATION ${BIN_INSTALL_DIR}/plugins)
