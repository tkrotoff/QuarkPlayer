project(MediaInfoTests)

macro(add_unit_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		MediaInfoFetcher

		${QT_QTCORE_LIBRARY}
		${QT_QTTEST_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_unit_test name)


set(MediaInfoTest_SRCS MediaInfoTest.cpp)
qt4_wrap_cpp(MediaInfoTest_SRCS MediaInfoTest.h)
add_unit_test(MediaInfoTest ${MediaInfoTest_SRCS})
