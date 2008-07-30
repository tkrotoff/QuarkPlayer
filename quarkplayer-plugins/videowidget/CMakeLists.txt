project(videowidget)

#Because of QtWebKit which contains not clean #include
include_directories(${QT_QTCORE_INCLUDE_DIR})
include_directories(${QT_QTWEBKIT_INCLUDE_DIR})

set(videowidget_SRCS
	VideoWidgetPlugin.cpp
	VideoWidget.cpp
	MediaDataWidget.cpp
	CoverArtWindow.cpp
)

qt4_wrap_cpp(videowidget_SRCS
	VideoWidgetPlugin.h
	VideoWidget.h
	MediaDataWidget.h
	CoverArtWindow.h
)

qt4_wrap_ui(videowidget_SRCS
	BackgroundLogoWidget.ui
	MediaDataWidget.ui
	CoverArtWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(videowidget MODULE ${videowidget_SRCS})

target_link_libraries(videowidget
	quarkplayercore
	contentfetcher
	webbrowser
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTWEBKIT_LIBRARY}
)

#Translations
add_qt_translations(${videowidget_SRCS})

install(TARGETS videowidget DESTINATION ${BIN_INSTALL_DIR}/plugins)
