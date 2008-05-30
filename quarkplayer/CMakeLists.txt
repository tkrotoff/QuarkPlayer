project(quarkplayer)

set(quarkplayer_SRCS
	main.cpp
	MainWindow.cpp
	PlayToolBar.cpp
	VideoWidget.cpp
	StatusBar.cpp
	MediaController.cpp
	QuarkPlayerStyle.cpp
	QuickSettingsWindow.cpp
	config/IConfig.cpp
	config/Config.cpp
	config/ConfigWindow.cpp
	config/GeneralConfigWidget.cpp
	config/SettingsBrowser.cpp
)

qt4_add_resources(quarkplayer_SRCS
	quarkplayer.qrc
)

qt4_wrap_cpp(quarkplayer_SRCS
	MainWindow.h
	PlayToolBar.h
	VideoWidget.h
	StatusBar.h
	MediaController.h
	QuarkPlayerStyle.h
	QuickSettingsWindow.h
	config/Config.h
	config/ConfigWindow.h
	config/GeneralConfigWidget.h
	config/SettingsBrowser.h
)

qt4_wrap_ui(quarkplayer_SRCS
	MainWindow.ui
	PlayToolBar.ui
	QuickSettingsWindow.ui
	config/ConfigWindow.ui
	config/GeneralConfigWidget.ui
	config/SettingsBrowser.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(quarkplayer ${quarkplayer_SRCS})

if (KDE4_FOUND)
	target_link_libraries(quarkplayer
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_PHONON_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(quarkplayer
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_PHONON_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS quarkplayer DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
