project(QuickSettings)

set(QuickSettings_SRCS
	QuickSettingsWindow.cpp
)

qt4_wrap_cpp(QuickSettings_SRCS
	QuickSettingsWindow.h
)

qt4_wrap_ui(QuickSettings_SRCS
	QuickSettingsWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (STATICPLUGINS)
	add_library(QuickSettings STATIC ${QuickSettings_SRCS})
else (STATICPLUGINS)
	add_library(QuickSettings MODULE ${QuickSettings_SRCS})
endif (STATICPLUGINS)

target_link_libraries(QuickSettings
	QuarkPlayerCore

	MainWindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${QuickSettings_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS QuickSettings ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
