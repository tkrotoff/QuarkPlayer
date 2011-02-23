project(Playlist)

add_definitions(-DBUILD_PLAYLIST)

set(Playlist_SRCS
	PlaylistWidget.cpp
	PlaylistModel.cpp
	PlaylistFilter.cpp
	DragAndDropTreeView.cpp
	PlaylistCommandLineParser.cpp
)

qt4_wrap_cpp(Playlist_SRCS
	PlaylistWidget.h
	PlaylistModel.h
	PlaylistFilter.h
	DragAndDropTreeView.h
)

if (STATICPLUGINS)
	add_library(Playlist STATIC ${Playlist_SRCS})
else (STATICPLUGINS)
	add_library(Playlist SHARED ${Playlist_SRCS})
endif (STATICPLUGINS)

target_link_libraries(Playlist
	TkUtil
	PlaylistParser
	MediaInfoWindow

	QuarkPlayerCore

	MainWindow
	ConfigWindow

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${Playlist_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS Playlist ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
