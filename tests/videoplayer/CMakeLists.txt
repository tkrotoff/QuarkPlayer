project(videoplayer)

set(videoplayer_SRCS
	main.cpp
	MainWindow.cpp
)

qt4_add_resources(videoplayer_SRCS
	videoplayer.qrc
)

qt4_wrap_cpp(videoplayer_SRCS
	MainWindow.h
)

qt4_wrap_ui(videoplayer_SRCS
	MainWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(videoplayer ${videoplayer_SRCS})

target_link_libraries(videoplayer
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_PHONON_LIBRARY}
)

install(TARGETS videoplayer DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
