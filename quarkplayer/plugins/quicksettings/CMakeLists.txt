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

if (KDE4_FOUND)
	target_link_libraries(quicksettings
		libquarkplayer
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(quicksettings
		libquarkplayer
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS quicksettings DESTINATION ${BIN_INSTALL_DIR}/plugins)
