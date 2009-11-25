project(TkUtil)

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

add_definitions(-DBUILD_TKUTIL)

set(TkUtil_SRCS
	TkFileDialog.cpp
	TkComboBox.cpp
	EventFilter.cpp
	LanguageChangeEventFilter.cpp
	MouseEventFilter.cpp
	CloseEventFilter.cpp
	KeyEventFilter.cpp
	DropEventFilter.cpp
	TkIcon.cpp
	ActionCollection.cpp
	TkConfig.cpp
	Translator.cpp
	TkStackedWidget.cpp
	FindFiles.cpp
	TkFile.cpp
	Random.cpp
	TkTime.cpp
	FileChooserWidget.cpp
	ScreenSaver.cpp
	SearchLineEdit.cpp
	LineEdit.cpp
	TkAction.cpp
	SqueezeLabel.cpp
)

qt4_wrap_cpp(TkUtil_SRCS
	EventFilter.h
	TkConfig.h
	FindFiles.h
	FileChooserWidget.h
	SearchLineEdit.h
	LineEdit.h
	LineEdit_p.h
	ProxyStyle.h
	TkAction.h
)

add_library(TkUtil SHARED ${TkUtil_SRCS})

if (KDE4_FOUND)
	target_link_libraries(TkUtil
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(TkUtil
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS TkUtil ${INSTALL_TARGETS_DEFAULT_ARGS})
