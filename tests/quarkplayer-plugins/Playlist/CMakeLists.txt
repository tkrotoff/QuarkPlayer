project(PlaylistTests)

macro(add_my_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		Playlist

		${QT_QTGUI_LIBRARY}
		${QT_QTTEST_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_my_test name)


set(PlaylistWidgetTest_SRCS PlaylistWidgetTest.cpp)
qt4_wrap_cpp(PlaylistWidgetTest_SRCS PlaylistWidgetTest.h)
add_my_test(PlaylistWidgetTest ${PlaylistWidgetTest_SRCS})
