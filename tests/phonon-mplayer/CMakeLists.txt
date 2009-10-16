project(QGraphicsProxyWidgetTest)

set(QGraphicsProxyWidgetTest_SRCS
	QGraphicsProxyWidgetTest.cpp
	Download.cpp
)

qt4_wrap_cpp(QGraphicsProxyWidgetTest_SRCS
	Download.h
)

add_executable(QGraphicsProxyWidgetTest ${QGraphicsProxyWidgetTest_SRCS})

target_link_libraries(QGraphicsProxyWidgetTest
	${QT_QTCORE_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_PHONON_LIBRARY}
	${QT_QTMAIN_LIBRARY}
)

install(TARGETS QGraphicsProxyWidgetTest ${INSTALL_TARGETS_DEFAULT_ARGS})
