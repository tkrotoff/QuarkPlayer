project(TkUtilTests)

macro(add_my_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		TkUtil

		${QT_QTCORE_LIBRARY}
		${QT_QTTEST_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_my_test name)


set(SearchLineEditTest_SRCS SearchLineEditTest.cpp)
qt4_wrap_cpp(SearchLineEditTest_SRCS SearchLineEditTest.h)
qt4_add_resources(SearchLineEditTest_SRCS SearchLineEditTest.qrc)
add_my_test(SearchLineEditTest ${SearchLineEditTest_SRCS})


set(FindFilesTest_SRCS FindFilesTest.cpp)
qt4_wrap_cpp(FindFilesTest_SRCS FindFilesTest.h)
add_my_test(FindFilesTest ${FindFilesTest_SRCS})
