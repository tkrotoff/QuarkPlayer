project(tkutil)

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

set(tkutil_SRCS
	TkFileDialog.cpp
	TkComboBox.cpp
	TkEventFilter.cpp
	TkLanguageChangeEventFilter.cpp
)

qt4_wrap_cpp(tkutil_SRCS
	TkEventFilter.h
	TkLanguageChangeEventFilter.h
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
