project(ConfigWindow)

add_definitions(-DBUILD_CONFIGWINDOW ${QTSINGLEAPPLICATION_DEFINITIONS})

set(ConfigWindow_SRCS
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

qt4_wrap_cpp(ConfigWindow_SRCS
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
)

qt4_wrap_ui(ConfigWindow_SRCS
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
	add_library(ConfigWindow STATIC ${ConfigWindow_SRCS})
else (STATICPLUGINS)
	add_library(ConfigWindow SHARED ${ConfigWindow_SRCS})
endif (STATICPLUGINS)

target_link_libraries(ConfigWindow
	QuarkPlayerCore

	MainWindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${ConfigWindow_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS ConfigWindow ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
