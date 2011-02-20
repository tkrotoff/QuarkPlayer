project(VideoWidgetTests)

macro(add_my_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		VideoWidget
		PlayToolBar

		${QT_QTGUI_LIBRARY}
		${QT_QTTEST_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_my_test name)


set(MyVideoWidgetTest_SRCS MyVideoWidgetTest.cpp)
qt4_wrap_cpp(MyVideoWidgetTest_SRCS MyVideoWidgetTest.h)
add_my_test(MyVideoWidgetTest ${MyVideoWidgetTest_SRCS})
