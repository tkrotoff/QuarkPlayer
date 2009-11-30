project(MediaInfoTest)

set(MediaInfoTest_SRCS
	MediaInfoTest.cpp
	main.cpp
)

qt4_wrap_cpp(MediaInfoTest_SRCS
	MediaInfoTest.h
)

add_executable(MediaInfoTest ${MediaInfoTest_SRCS})
add_test(MediaInfoTest MediaInfoTest)

target_link_libraries(MediaInfoTest
	MediaInfoFetcher

	${QT_QTCORE_LIBRARY}
	${QT_QTTEST_LIBRARY}
)

install(TARGETS MediaInfoTest ${INSTALL_TARGETS_DEFAULT_ARGS})
