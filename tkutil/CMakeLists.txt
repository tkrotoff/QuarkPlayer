project(tkutil)

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

add_definitions(-DBUILD_TKUTIL)

set(tkutil_SRCS
	TkFileDialog.cpp
	TkComboBox.cpp
	EventFilter.cpp
	LanguageChangeEventFilter.cpp
	MouseEventFilter.cpp
	CloseEventFilter.cpp
	TkIcon.cpp
	ActionCollection.cpp
	TkConfig.cpp
	Translator.cpp
	TkStackedWidget.cpp
	FindFiles.cpp
)

qt4_wrap_cpp(tkutil_SRCS
	EventFilter.h
	LanguageChangeEventFilter.h
	MouseEventFilter.h
	CloseEventFilter.h
	TkConfig.h
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
	)
endif (KDE4_FOUND)

install(TARGETS tkutil DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
