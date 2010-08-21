project(MainWindowTest)

macro(add_unit_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		QuarkPlayerCore

		MainWindow

		${QT_QTTEST_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_unit_test name)


set(QDockWidgetTest_SRCS
	QDockWidgetTest.cpp
)
qt4_wrap_cpp(QDockWidgetTest_SRCS
	QDockWidgetTest.h
)
add_unit_test(QDockWidgetTest ${QDockWidgetTest_SRCS})


set(LogWindowTest_SRCS
	LogWindowTest.cpp
)
qt4_wrap_cpp(LogWindowTest_SRCS
	LogWindowTest.h
)
add_unit_test(LogWindowTest ${LogWindowTest_SRCS})
