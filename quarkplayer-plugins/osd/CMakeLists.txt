project(osd)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(osd_SRCS
	OSDWidget.cpp
)

qt4_wrap_cpp(osd_SRCS
	OSDWidget.h
)

qt4_wrap_ui(osd_SRCS
	OSDWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (STATICPLUGINS)
	add_library(osd STATIC ${osd_SRCS})
else (STATICPLUGINS)
	add_library(osd SHARED ${osd_SRCS})
endif (STATICPLUGINS)

target_link_libraries(osd
	quarkplayercore
	mainwindow
	mediainfowindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${osd_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS osd ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
