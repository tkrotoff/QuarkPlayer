project(QuarkPlayerCoreTests)

macro(add_unit_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		QuarkPlayerCore

		${QT_QTCORE_LIBRARY}
		${QT_QTTEST_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_unit_test name)


set(LogMessageTest_SRCS
	LogMessageTest.cpp
)
qt4_wrap_cpp(LogMessageTest_SRCS
	LogMessageTest.h
)
add_unit_test(LogMessageTest ${LogMessageTest_SRCS})


set(LogModelTest_SRCS
	LogModelTest.cpp
)
qt4_wrap_cpp(LogModelTest_SRCS
	LogModelTest.h
)
qt4_add_resources(LogModelTest_SRCS
	LogModelTest.qrc
)
add_unit_test(LogModelTest ${LogModelTest_SRCS})


set(LogMessageHandlerTest_SRCS
	LogMessageHandlerTest.cpp
)
qt4_wrap_cpp(LogMessageHandlerTest_SRCS
	LogMessageHandlerTest.h
)
add_unit_test(LogMessageHandlerTest ${LogMessageHandlerTest_SRCS})
