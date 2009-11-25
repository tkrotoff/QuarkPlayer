project(PlayToolBar)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(PlayToolBar_SRCS
	PlayToolBar.cpp
)

qt4_wrap_cpp(PlayToolBar_SRCS
	PlayToolBar.h
)

if (STATICPLUGINS)
	add_library(PlayToolBar STATIC ${PlayToolBar_SRCS})
else (STATICPLUGINS)
	add_library(PlayToolBar MODULE ${PlayToolBar_SRCS})
endif (STATICPLUGINS)

target_link_libraries(PlayToolBar
	QuarkPlayerCore

	MainWindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${PlayToolBar_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS PlayToolBar ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
