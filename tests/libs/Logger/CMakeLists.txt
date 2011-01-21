project(LoggerTests)

macro(add_unit_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		Logger

		${QT_QTGUI_LIBRARY}
		${QT_QTTEST_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_unit_test name)


set(LogMessageTest_SRCS LogMessageTest.cpp)
qt4_wrap_cpp(LogMessageTest_SRCS LogMessageTest.h)
add_unit_test(LogMessageTest ${LogMessageTest_SRCS})


set(LogModelTest_SRCS LogModelTest.cpp)
qt4_wrap_cpp(LogModelTest_SRCS LogModelTest.h)
qt4_add_resources(LogModelTest_SRCS LogModelTest.qrc)
add_unit_test(LogModelTest ${LogModelTest_SRCS})


set(LoggerTest_SRCS LoggerTest.cpp)
qt4_wrap_cpp(LoggerTest_SRCS LoggerTest.h)
add_unit_test(LoggerTest ${LoggerTest_SRCS})


set(LogWindowTest_SRCS LogWindowTest.cpp)
qt4_wrap_cpp(LogWindowTest_SRCS LogWindowTest.h)
add_unit_test(LogWindowTest ${LogWindowTest_SRCS})


set(ConsoleOutputTest_SRCS ConsoleOutputTest.cpp)
qt4_wrap_cpp(ConsoleOutputTest_SRCS ConsoleOutputTest.h)
add_unit_test(ConsoleOutputTest ${ConsoleOutputTest_SRCS})
