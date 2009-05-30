project(configwindow)

add_definitions(-DQT_QTSINGLEAPPLICATION_IMPORT)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(configwindow_SRCS
	ConfigWindowPlugin.cpp
	ConfigWindow.cpp
	GeneralConfigWidget.cpp
	SettingsBrowser.cpp
	BackendCapabilitiesWidget.cpp
	PluginConfigWidget.cpp
	WinFileAssociationsConfigWidget.cpp
	PlaylistConfig.cpp
	ShortcutsConfigWidget.cpp
	ShortcutsFileParser.cpp
	ShortcutsConfig.cpp
)

qt4_wrap_cpp(configwindow_SRCS
	ConfigWindowPlugin.h
	ConfigWindow.h
	IConfigWidget.h
	GeneralConfigWidget.h
	SettingsBrowser.h
	BackendCapabilitiesWidget.h
	PluginConfigWidget.h
	WinFileAssociationsConfigWidget.h
	PlaylistConfig.h
	ShortcutsConfigWidget.h
	ShortcutsConfig.h
)

qt4_wrap_ui(configwindow_SRCS
	ConfigWindow.ui
	GeneralConfigWidget.ui
	SettingsBrowser.ui
	BackendCapabilitiesWidget.ui
	PluginConfigWidget.ui
	WinFileAssociationsConfigWidget.ui
	ShortcutsConfigWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (STATICPLUGINS)
	add_library(configwindow STATIC ${configwindow_SRCS})
else (STATICPLUGINS)
	add_library(configwindow SHARED ${configwindow_SRCS})
endif (STATICPLUGINS)

target_link_libraries(configwindow
	quarkplayercore
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${configwindow_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS configwindow ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
