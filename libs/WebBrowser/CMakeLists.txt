project(WebBrowser)

add_definitions(-DBUILD_WEBBROWSER)

if (WEBKIT)
	# Bug inside qwebelement.h with Qt 4.6.1: includes <QString> instead of <QtCore/QString>
	include_directories(${QT_QTCORE_INCLUDE_DIR})

	add_definitions(-DWEBKIT)
endif (WEBKIT)

set(WebBrowser_SRCS
	WebBrowser.cpp
	TkTextBrowser.cpp
)

qt4_wrap_cpp(WebBrowser_SRCS
	WebBrowser.h
	TkTextBrowser.h
)

add_library(WebBrowser SHARED ${WebBrowser_SRCS})

target_link_libraries(WebBrowser
	TkUtil

	${QT_QTCORE_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

if (WEBKIT)
	target_link_libraries(WebBrowser ${QT_QTWEBKIT_LIBRARY})
endif (WEBKIT)

#Translations
add_qt_translations(${WebBrowser_SRCS})

install(TARGETS WebBrowser ${INSTALL_TARGETS_DEFAULT_ARGS})
