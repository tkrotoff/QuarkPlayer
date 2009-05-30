project(playlist)

add_definitions(-DBUILD_PLAYLIST)

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(playlist_SRCS
	PlaylistWidget.cpp
	PlaylistModel.cpp
	PlaylistFilter.cpp
	DragAndDropTreeView.cpp
	PlaylistCommandLineParser.cpp
)

qt4_wrap_cpp(playlist_SRCS
	PlaylistWidget.h
	PlaylistModel.h
	PlaylistFilter.h
	DragAndDropTreeView.h
	PlaylistCommandLineParser.h
)

if (STATICPLUGINS)
	add_library(playlist STATIC ${playlist_SRCS})
else (STATICPLUGINS)
	add_library(playlist SHARED ${playlist_SRCS})
endif (STATICPLUGINS)

target_link_libraries(playlist
	quarkplayercore
	configwindow
	tkutil
	playlistparser
	mediainfowindow
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

# Translations
add_qt_translations(${playlist_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS playlist ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
