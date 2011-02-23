project(ConfigWindowTests)

macro(add_my_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		MainWindow
		ConfigWindow

		${QT_QTGUI_LIBRARY}
		${QT_QTTEST_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_my_test name)


set(ConfigWindowPluginTest_SRCS ConfigWindowPluginTest.cpp)
qt4_wrap_cpp(ConfigWindowPluginTest_SRCS ConfigWindowPluginTest.h)
add_my_test(ConfigWindowPluginTest ${ConfigWindowPluginTest_SRCS})
