project(filebrowser)

add_definitions(-DBUILD_FILEBROWSER)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(filebrowser_SRCS
	FileBrowserWidget.cpp
	FileBrowserTreeView.cpp
	FileSearchModel.cpp
	FileSearchItem.cpp
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

if (STATICPLUGINS)
	add_library(filebrowser STATIC ${filebrowser_SRCS})
else (STATICPLUGINS)
	add_library(filebrowser SHARED ${filebrowser_SRCS})
endif (STATICPLUGINS)

target_link_libraries(filebrowser
	quarkplayercore
	filetypes
	tkutil
	mediainfowindow
	modeltest
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${filebrowser_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS filebrowser ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
