project(filebrowser)

set(filebrowser_SRCS
	FileBrowserWidget.cpp
	FileBrowserTreeView.cpp
	FileSearchModel.cpp
	Track.cpp
	UuidActionCollection.cpp
	config/FileBrowserConfigWidget.cpp
)

qt4_wrap_cpp(filebrowser_SRCS
	FileBrowserWidget.h
	FileBrowserTreeView.h
	FileSearchModel.h
	config/FileBrowserConfigWidget.h
)

qt4_wrap_ui(filebrowser_SRCS
	config/FileBrowserConfigWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(filebrowser MODULE ${filebrowser_SRCS})

target_link_libraries(filebrowser
	quarkplayercore
	filetypes
	tkutil
	mediainfowindow
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

#Translations
add_qt_translations(${filebrowser_SRCS})

install(TARGETS filebrowser DESTINATION ${BIN_INSTALL_DIR}/plugins)
