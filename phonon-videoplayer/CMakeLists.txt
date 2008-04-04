project(videoplayer)

find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDE_DIR})

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

if (UNIX)
	set(QT_PHONON_LIBRARY_DEBUG
		${QT_LIBRARY_DIR}/libphonon.so.4.1.0.debug
	)
endif (UNIX)

if (WIN32)
	set(QT_PHONON_LIBRARY_DEBUG
		${QT_LIBRARY_DIR}/phonond4.lib
	)
endif (WIN32)

target_link_libraries(videoplayer
	${QT_QTCORE_LIBRARY_DEBUG}
	${QT_QTGUI_LIBRARY_DEBUG}
	${QT_PHONON_LIBRARY_DEBUG}
)
