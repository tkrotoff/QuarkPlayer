project(FindFilesTest)

set(FindFilesTest_SRCS
	FindFilesTest.cpp
)

qt4_wrap_cpp(FindFilesTest_SRCS
	FindFilesTest.h
)

add_executable(FindFilesTest ${FindFilesTest_SRCS})
add_test(FindFilesTest FindFilesTest)

target_link_libraries(FindFilesTest
	TkUtil

	${QT_QTCORE_LIBRARY}
	${QT_QTTEST_LIBRARY}
)

install(TARGETS FindFilesTest ${INSTALL_TARGETS_DEFAULT_ARGS})
