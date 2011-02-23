project(VideoWidget)

set(VideoWidget_SRCS
	VideoWidgetPlugin.cpp
	MyVideoWidget.cpp
	MediaDataWidget.cpp
)

qt4_wrap_cpp(VideoWidget_SRCS
	VideoWidgetPlugin.h
	MyVideoWidget.h
	MediaDataWidget.h
)

qt4_wrap_ui(VideoWidget_SRCS
	BackgroundLogoWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (STATICPLUGINS)
	add_library(VideoWidget STATIC ${VideoWidget_SRCS})
else (STATICPLUGINS)
	add_library(VideoWidget MODULE ${VideoWidget_SRCS})
endif (STATICPLUGINS)

target_link_libraries(VideoWidget
	QuarkPlayerCore
	MainWindow
	MediaInfoWindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${VideoWidget_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS VideoWidget ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
