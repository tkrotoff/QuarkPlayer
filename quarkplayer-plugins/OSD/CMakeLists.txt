project(OSD)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(OSD_SRCS
	OSDWidget.cpp
)

qt4_wrap_cpp(OSD_SRCS
	OSDWidget.h
)

if (STATICPLUGINS)
	add_library(OSD STATIC ${OSD_SRCS})
else (STATICPLUGINS)
	add_library(OSD SHARED ${OSD_SRCS})
endif (STATICPLUGINS)

target_link_libraries(OSD
	QuarkPlayerCore
	MainWindow
	MediaInfoWindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${OSD_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS OSD ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
