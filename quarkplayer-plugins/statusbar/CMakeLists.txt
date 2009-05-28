project(statusbar)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(statusbar_SRCS
	StatusBar.cpp
)

qt4_wrap_cpp(statusbar_SRCS
	StatusBar.h
)

if (STATICPLUGINS)
	add_library(statusbar STATIC ${statusbar_SRCS})
else (STATICPLUGINS)
	add_library(statusbar MODULE ${statusbar_SRCS})
endif (STATICPLUGINS)

target_link_libraries(statusbar
	quarkplayercore
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${statusbar_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS statusbar ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
