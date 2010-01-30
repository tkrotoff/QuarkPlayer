project(PlaylistParser)

add_definitions(-DBUILD_PLAYLISTPARSER)

set(PlaylistParser_SRCS
	IPlaylistParser.cpp
	PlaylistParser.cpp
	IPlaylistParserImpl.cpp

	Util.cpp

	M3UParser.cpp
	WPLParser.cpp
	PLSParser.cpp
	ASXParser.cpp
	XSPFParser.cpp
	CUEParser.cpp
)

qt4_wrap_cpp(PlaylistParser_SRCS
	PlaylistParser.h
	IPlaylistParserImpl.h
)

add_library(PlaylistParser SHARED ${PlaylistParser_SRCS})

target_link_libraries(PlaylistParser
	MediaInfoFetcher
	TkUtil

	${QT_QTCORE_LIBRARY}
	${QT_QTXML_LIBRARY}
)

install(TARGETS PlaylistParser ${INSTALL_TARGETS_DEFAULT_ARGS})
