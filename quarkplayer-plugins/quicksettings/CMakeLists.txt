project(quicksettings)

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

add_library(quicksettings MODULE ${quicksettings_SRCS})

target_link_libraries(quicksettings
	quarkplayercore
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

#Translations
add_qt_translations(${quicksettings_SRCS})

install(TARGETS quicksettings DESTINATION ${BIN_INSTALL_DIR}/plugins)
