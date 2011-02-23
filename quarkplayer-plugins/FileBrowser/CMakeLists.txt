project(FileBrowser)

add_definitions(-DBUILD_FILEBROWSER)

set(FileBrowser_SRCS
	FileBrowserWidget.cpp
	FileBrowserTreeView.cpp
	FileSearchModel.cpp
	FileSearchItem.cpp
	config/FileBrowserConfigWidget.cpp
)

qt4_wrap_cpp(FileBrowser_SRCS
	FileBrowserWidget.h
	FileBrowserTreeView.h
	FileSearchModel.h
	config/FileBrowserConfigWidget.h
)

qt4_wrap_ui(FileBrowser_SRCS
	config/FileBrowserConfigWidget.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (STATICPLUGINS)
	add_library(FileBrowser STATIC ${FileBrowser_SRCS})
else (STATICPLUGINS)
	add_library(FileBrowser SHARED ${FileBrowser_SRCS})
endif (STATICPLUGINS)

target_link_libraries(FileBrowser
	QuarkPlayerCore

	MainWindow
	ConfigWindow
	Playlist

	FileTypes
	TkUtil
	MediaInfoWindow

	modeltest

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${FileBrowser_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS FileBrowser ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
