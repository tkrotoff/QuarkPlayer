project(mediacontroller)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(mediacontroller_SRCS
	MediaController.cpp
	MediaControllerToolBar.cpp
)

qt4_wrap_cpp(mediacontroller_SRCS
	MediaController.h
	MediaControllerToolBar.h
)

if (STATICPLUGINS)
	add_library(mediacontroller STATIC ${mediacontroller_SRCS})
else (STATICPLUGINS)
	add_library(mediacontroller MODULE ${mediacontroller_SRCS})
endif (STATICPLUGINS)

target_link_libraries(mediacontroller
	quarkplayercore
	filetypes
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${mediacontroller_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS mediacontroller ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
