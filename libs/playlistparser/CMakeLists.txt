project(playlistparser)

add_definitions(-DBUILD_PLAYLISTPARSER)

set(playlistparser_SRCS
	PlaylistParser.cpp
	PlaylistParserThread.cpp

	Util.cpp

	M3UParser.cpp
	WPLParser.cpp
	PLSParser.cpp
)

qt4_wrap_cpp(playlistparser_SRCS
	IPlaylistParser.h
	PlaylistParserThread.h
)

add_library(playlistparser SHARED ${playlistparser_SRCS})

target_link_libraries(playlistparser
	mediainfowindow
	${QT_QTCORE_LIBRARY}
	${QT_QTXML_LIBRARY}
)

install(TARGETS playlistparser DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
