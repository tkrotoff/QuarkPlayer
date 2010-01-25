project(QDockWidgetTest)

set(QDockWidgetTest_SRCS
	QDockWidgetTest.cpp
)

qt4_wrap_cpp(QDockWidgetTest_SRCS
	QDockWidgetTest.h
)

add_executable(QDockWidgetTest ${QDockWidgetTest_SRCS})
add_test(QDockWidgetTest QDockWidgetTest)

target_link_libraries(QDockWidgetTest
	TkUtil

	${QT_QTGUI_LIBRARY}
	${QT_QTCORE_LIBRARY}
	${QT_QTTEST_LIBRARY}
)

install(TARGETS QDockWidgetTest ${INSTALL_TARGETS_DEFAULT_ARGS})
