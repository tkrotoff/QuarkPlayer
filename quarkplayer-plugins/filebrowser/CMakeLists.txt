project(filebrowser)

set(filebrowser_SRCS
	FileBrowserWidget.cpp
	SimpleDirModel.cpp
	config/FileBrowserConfigWidget.cpp
)

qt4_wrap_cpp(filebrowser_SRCS
	FileBrowserWidget.h
	config/FileBrowserConfigWidget.h
)

qt4_wrap_ui(filebrowser_SRCS
	config/FileBrowserConfigWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(filebrowser MODULE ${filebrowser_SRCS})

if (KDE4_FOUND)
	target_link_libraries(filebrowser
		quarkplayercore
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_KIO_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(filebrowser
		quarkplayercore
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
endif (KDE4_FOUND)

#Translations
add_qt_translations(${filebrowser_SRCS})

install(TARGETS filebrowser DESTINATION ${BIN_INSTALL_DIR}/plugins)
