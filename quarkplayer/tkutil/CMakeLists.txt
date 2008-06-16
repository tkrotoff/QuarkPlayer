project(tkutil)

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

set(tkutil_SRCS
	TkFileDialog.cpp
	TkComboBox.cpp
	EventFilter.cpp
	LanguageChangeEventFilter.cpp
	TkIcon.cpp
	ActionCollection.cpp
	IConfig.cpp
	Translator.cpp
	TkStackedWidget.cpp
)

qt4_wrap_cpp(tkutil_SRCS
	EventFilter.h
	LanguageChangeEventFilter.h
)

add_library(tkutil STATIC ${tkutil_SRCS})

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
	)
endif (KDE4_FOUND)
