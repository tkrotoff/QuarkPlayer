project(playlistparser)

add_definitions(-DBUILD_PLAYLISTPARSER)

set(playlistparser_SRCS
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

qt4_wrap_cpp(playlistparser_SRCS
	IPlaylistParser.h
	PlaylistParser.h
	IPlaylistParserImpl.h
)

add_library(playlistparser SHARED ${playlistparser_SRCS})

target_link_libraries(playlistparser
	mediainfofetcher
	tkutil

	${QT_QTCORE_LIBRARY}
	${QT_QTXML_LIBRARY}
)

install(TARGETS playlistparser ${INSTALL_TARGETS_DEFAULT_ARGS})
