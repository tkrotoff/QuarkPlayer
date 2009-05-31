project(playtoolbar)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(playtoolbar_SRCS
	PlayToolBar.cpp
)

qt4_wrap_cpp(playtoolbar_SRCS
	PlayToolBar.h
)

if (STATICPLUGINS)
	add_library(playtoolbar STATIC ${playtoolbar_SRCS})
else (STATICPLUGINS)
	add_library(playtoolbar MODULE ${playtoolbar_SRCS})
endif (STATICPLUGINS)

target_link_libraries(playtoolbar
	quarkplayercore

	mainwindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${playtoolbar_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS playtoolbar ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
