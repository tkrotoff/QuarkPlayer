project(videowidget)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

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

if (STATICPLUGINS)
	add_library(videowidget STATIC ${videowidget_SRCS})
else (STATICPLUGINS)
	add_library(videowidget MODULE ${videowidget_SRCS})
endif (STATICPLUGINS)

target_link_libraries(videowidget
	quarkplayercore
	mainwindow

	mediainfowindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${videowidget_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS videowidget ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
