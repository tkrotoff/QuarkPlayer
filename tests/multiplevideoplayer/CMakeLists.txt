project(multiplevideoplayer)

set(multiplevideoplayer_SRCS
	main.cpp
	MainWindow.cpp
	VideoWindow.cpp
)

qt4_add_resources(multiplevideoplayer_SRCS
	multiplevideoplayer.qrc
)

qt4_wrap_cpp(multiplevideoplayer_SRCS
	MainWindow.h
	VideoWindow.h
)

qt4_wrap_ui(multiplevideoplayer_SRCS
	MainWindow.ui
	VideoWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(multiplevideoplayer ${multiplevideoplayer_SRCS})

target_link_libraries(multiplevideoplayer
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_PHONON_LIBRARY}
)

install(TARGETS multiplevideoplayer ${INSTALL_TARGETS_DEFAULT_ARGS})
