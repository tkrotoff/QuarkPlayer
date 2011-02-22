project(FindSubtitlesTests)

macro(add_my_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		FindSubtitles

		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTTEST_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_my_test name)


set(FindSubtitlesTest_SRCS FindSubtitlesTest.cpp)
qt4_wrap_cpp(FindSubtitlesTest_SRCS FindSubtitlesTest.h)
add_my_test(FindSubtitlesTest ${FindSubtitlesTest_SRCS})


set(ZipFileTest_SRCS ZipFileTest.cpp)
qt4_wrap_cpp(ZipFileTest_SRCS ZipFileTest.h)
qt4_add_resources(ZipFileTest_SRCS zipfiles.qrc)
add_my_test(ZipFileTest ${ZipFileTest_SRCS})
