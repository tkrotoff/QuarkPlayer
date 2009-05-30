project(welcome)

set(welcome_SRCS
	WelcomeWindow.cpp
)

qt4_wrap_cpp(welcome_SRCS
	WelcomeWindow.h
)

add_library(welcome MODULE ${welcome_SRCS})

target_link_libraries(welcome
	quarkplayercore

	mainwindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${welcome_SRCS})

install(TARGETS welcome ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
