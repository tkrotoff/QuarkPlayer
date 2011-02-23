project(MainWindowTests)

macro(add_my_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		MainWindow

		${QT_QTGUI_LIBRARY}
		${QT_QTTEST_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_my_test name)


set(MainWindowTest_SRCS MainWindowTest.cpp)
qt4_wrap_cpp(MainWindowTest_SRCS MainWindowTest.h)
add_my_test(MainWindowTest ${MainWindowTest_SRCS})
