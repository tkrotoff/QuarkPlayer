project(multiplevideoplayer)

find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDE_DIR})

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

target_link_libraries(multiplevideoplayer
	${QT_QTCORE_LIBRARY_DEBUG}
	${QT_QTGUI_LIBRARY_DEBUG}
	${QT_PHONON_LIBRARY_DEBUG}
)
