project(tkutil)

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

add_definitions(-DBUILD_TKUTIL -DUNICODE)

set(tkutil_SRCS
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
	TkTextBrowser.cpp
	SyncHttp.cpp
	TkTime.cpp
	FileChooserWidget.cpp
	ScreenSaver.cpp
	SearchLineEdit.cpp
	LineEdit.cpp
	TkAction.cpp
	SqueezeLabel.cpp
)

qt4_wrap_cpp(tkutil_SRCS
	EventFilter.h
	TkConfig.h
	FindFiles.h
	TkTextBrowser.h
	SyncHttp.h
	FileChooserWidget.h
	SearchLineEdit.h
	LineEdit.h
	LineEdit_p.h
	ProxyStyle.h
	TkAction.h
)

add_library(tkutil SHARED ${tkutil_SRCS})

if (KDE4_FOUND)
	target_link_libraries(tkutil
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(tkutil
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS tkutil DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
