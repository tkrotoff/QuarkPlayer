project(webbrowser)

add_definitions(-DBUILD_WEBBROWSER)

set(webbrowser_SRCS
	WebBrowser.cpp
)

qt4_wrap_cpp(webbrowser_SRCS
	WebBrowser.h
)

#add_library(webbrowser SHARED ${webbrowser_SRCS})
add_library(webbrowser STATIC ${webbrowser_SRCS})

target_link_libraries(webbrowser
	tkutil
	${QT_QTCORE_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

install(TARGETS webbrowser DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
