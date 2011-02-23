project(Welcome)

set(Welcome_SRCS
	WelcomeWindow.cpp
)

qt4_wrap_cpp(Welcome_SRCS
	WelcomeWindow.h
)

if (STATICPLUGINS)
	add_library(Welcome STATIC ${Welcome_SRCS})
else (STATICPLUGINS)
	add_library(Welcome MODULE ${Welcome_SRCS})
endif (STATICPLUGINS)

target_link_libraries(Welcome
	QuarkPlayerCore

	MainWindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${Welcome_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS Welcome ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
