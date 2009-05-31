project(quicksettings)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(quicksettings_SRCS
	QuickSettingsWindow.cpp
)

qt4_wrap_cpp(quicksettings_SRCS
	QuickSettingsWindow.h
)

qt4_wrap_ui(quicksettings_SRCS
	QuickSettingsWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (STATICPLUGINS)
	add_library(quicksettings STATIC ${quicksettings_SRCS})
else (STATICPLUGINS)
	add_library(quicksettings MODULE ${quicksettings_SRCS})
endif (STATICPLUGINS)

target_link_libraries(quicksettings
	quarkplayercore

	mainwindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${quicksettings_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS quicksettings ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
