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
	MediaDataWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(videowidget MODULE ${videowidget_SRCS})

target_link_libraries(videowidget
	quarkplayercore
	mediainfowindow
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${videowidget_SRCS})

install(TARGETS videowidget ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
