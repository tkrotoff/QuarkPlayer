project(WebBrowserTests)

macro(add_my_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		WebBrowser
		ContentFetcher

		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_my_test name)


set(WebBrowserTest_SRCS WebBrowserTest.cpp)
qt4_wrap_cpp(WebBrowserTest_SRCS WebBrowserTest.h)
add_my_test(WebBrowserTest ${WebBrowserTest_SRCS})
