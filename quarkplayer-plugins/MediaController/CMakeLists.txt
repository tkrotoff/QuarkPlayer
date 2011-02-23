project(MediaController)

add_definitions(-DBUILD_MEDIACONTROLLER)

set(MediaController_SRCS
	MediaController.cpp
	MediaControllerToolBar.cpp
)

qt4_wrap_cpp(MediaController_SRCS
	MediaController.h
	MediaControllerToolBar.h
)

if (STATICPLUGINS)
	add_library(MediaController STATIC ${MediaController_SRCS})
else (STATICPLUGINS)
	add_library(MediaController SHARED ${MediaController_SRCS})
endif (STATICPLUGINS)

target_link_libraries(MediaController
	FileTypes

	QuarkPlayerCore

	Playlist
	MainWindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${MediaController_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS MediaController ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
