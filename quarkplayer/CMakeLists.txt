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
	ActionCollection.cpp
	MyIcon.cpp
	IconEngine.cpp
	ComboBoxUtil.cpp
	config/IConfig.cpp
	config/Config.cpp
	config/ConfigWindow.cpp
	config/GeneralConfigWidget.cpp
	config/SettingsBrowser.cpp
)

qt4_add_resources(quarkplayer_SRCS
	quarkplayer.qrc
	quarkplayer_oxygen.qrc
	quarkplayer_silk.qrc
)

qt4_wrap_cpp(quarkplayer_SRCS
	MainWindow.h
	PlayToolBar.h
	VideoWidget.h
	StatusBar.h
	MediaController.h
	QuarkPlayerStyle.h
	QuickSettingsWindow.h
	ActionCollection.h
	MyIcon.h
	IconEngine.h
	ComboBoxUtil.h
	config/Config.h
	config/ConfigWindow.h
	config/GeneralConfigWidget.h
	config/SettingsBrowser.h
)

if (WIN32)
	list(APPEND quarkplayer_SRCS
		quarkplayer.rc
	)
endif (WIN32)

qt4_wrap_ui(quarkplayer_SRCS
	MainWindow.ui
	QuickSettingsWindow.ui
	BackgroundLogoWidget.ui
	config/ConfigWindow.ui
	config/GeneralConfigWidget.ui
	config/SettingsBrowser.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(quarkplayer_TRANSLATIONS
	${quarkplayer_SRCS}
	translations/quarkplayer_fr.ts
)
#qt4_create_translation(quarkplayer_QMS ${quarkplayer_TRANSLATIONS})
#qt4_add_translation(quarkplayer_QMS ${quarkplayer_TRANSLATIONS})

add_executable(quarkplayer ${quarkplayer_SRCS} ${quarkplayer_QMS})

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
install(FILES ${quarkplayer_QMS} DESTINATION ${BIN_INSTALL_DIR}/translations)
