project(StatusBar)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(StatusBar_SRCS
	StatusBar.cpp
)

qt4_wrap_cpp(StatusBar_SRCS
	StatusBar.h
)

if (STATICPLUGINS)
	add_library(StatusBar STATIC ${StatusBar_SRCS})
else (STATICPLUGINS)
	add_library(StatusBar MODULE ${StatusBar_SRCS})
endif (STATICPLUGINS)

target_link_libraries(StatusBar
	QuarkPlayerCore

	MainWindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${StatusBar_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS StatusBar ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
