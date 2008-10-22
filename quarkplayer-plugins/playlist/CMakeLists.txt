project(playlist)

set(playlist_SRCS
	PlaylistWidget.cpp
	PlaylistModel.cpp
	PlaylistFilter.cpp
	Track.cpp
	DragAndDropTreeView.cpp
	SearchToolBar.cpp
	UuidActionCollection.cpp
)

qt4_wrap_cpp(playlist_SRCS
	PlaylistWidget.h
	PlaylistModel.h
	PlaylistFilter.h
	DragAndDropTreeView.h
	SearchToolBar.h
)

add_library(playlist MODULE ${playlist_SRCS})

target_link_libraries(playlist
	quarkplayercore
	tkutil
	playlistparser
	mediainfowindow
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

#Translations
add_qt_translations(${playlist_SRCS})

install(TARGETS playlist DESTINATION ${BIN_INSTALL_DIR}/plugins)
