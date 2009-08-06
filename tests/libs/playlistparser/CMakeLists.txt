project(playlistparsertest)

set(playlistparsertest_SRCS
	PlaylistParserTest.cpp
)

qt4_wrap_cpp(playlistparsertest_SRCS
	PlaylistParserTest.h
)

add_executable(playlistparsertest ${playlistparsertest_SRCS})
add_test(PlaylistParserTest playlistparsertest)

target_link_libraries(playlistparsertest
	playlistparser

	${QT_QTCORE_LIBRARY}
	${QT_QTTEST_LIBRARY}
)

install(TARGETS playlistparsertest ${INSTALL_TARGETS_DEFAULT_ARGS})
