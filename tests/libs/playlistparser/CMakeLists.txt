project(PlaylistParserTest)

set(PlaylistParserTest_SRCS
	PlaylistParserTest.cpp
)

qt4_wrap_cpp(PlaylistParserTest_SRCS
	PlaylistParserTest.h
)

qt4_add_resources(PlaylistParserTest_SRCS
	playlists.qrc
)

add_executable(PlaylistParserTest ${PlaylistParserTest_SRCS})
add_test(PlaylistParserTest PlaylistParserTest)

target_link_libraries(PlaylistParserTest
	PlaylistParser

	${QT_QTCORE_LIBRARY}
	${QT_QTTEST_LIBRARY}
)

install(TARGETS PlaylistParserTest ${INSTALL_TARGETS_DEFAULT_ARGS})
