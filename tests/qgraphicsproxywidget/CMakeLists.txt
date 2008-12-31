project(testQGraphicsProxyWidget)

add_executable(testQGraphicsProxyWidget testQGraphicsProxyWidget.cpp)

target_link_libraries(testQGraphicsProxyWidget
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_PHONON_LIBRARY}
	${QT_QTMAIN_LIBRARY}
)

install(TARGETS testQGraphicsProxyWidget DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
