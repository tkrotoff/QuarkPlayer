project(PlaylistParserTests)

macro(add_unit_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		PlaylistParser

		${QT_QTCORE_LIBRARY}
		${QT_QTTEST_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_unit_test name)


set(PlaylistReaderTest_SRCS PlaylistReaderTest.cpp)
qt4_wrap_cpp(PlaylistReaderTest_SRCS PlaylistReaderTest.h)
qt4_add_resources(PlaylistReaderTest_SRCS playlists.qrc)
add_unit_test(PlaylistReaderTest ${PlaylistReaderTest_SRCS})


set(PlaylistWriterTest_SRCS PlaylistWriterTest.cpp)
qt4_wrap_cpp(PlaylistWriterTest_SRCS PlaylistWriterTest.h)
add_unit_test(PlaylistWriterTest ${PlaylistWriterTest_SRCS})
