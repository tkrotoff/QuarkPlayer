project(welcome)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(welcome_SRCS
	WelcomeWindow.cpp
)

qt4_wrap_cpp(welcome_SRCS
	WelcomeWindow.h
)

if (STATICPLUGINS)
	add_library(welcome STATIC ${welcome_SRCS})
else (STATICPLUGINS)
	add_library(welcome MODULE ${welcome_SRCS})
endif (STATICPLUGINS)

target_link_libraries(welcome
	quarkplayercore

	mainwindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${welcome_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS welcome ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
