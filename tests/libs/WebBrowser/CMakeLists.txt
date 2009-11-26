project(WebBrowserTest)

set(WebBrowserTest_SRCS
	WebBrowserTest.cpp
)

qt4_wrap_cpp(WebBrowserTest_SRCS
	WebBrowserTest.h
)

add_executable(WebBrowserTest ${WebBrowserTest_SRCS})

target_link_libraries(WebBrowserTest
	WebBrowser
	ContentFetcher

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

install(TARGETS WebBrowserTest ${INSTALL_TARGETS_DEFAULT_ARGS})
