project(webbrowsertest)

set(webbrowsertest_SRCS
	WebBrowserTest.cpp
)

qt4_wrap_cpp(webbrowsertest_SRCS
	WebBrowserTest.h
)

add_executable(webbrowsertest ${webbrowsertest_SRCS})

target_link_libraries(webbrowsertest
	webbrowser
	contentfetcher

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

install(TARGETS webbrowsertest ${INSTALL_TARGETS_DEFAULT_ARGS})
