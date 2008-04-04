project(mediaplayer)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDE_DIR})

set(mediaplayer_SRCS
	main.cpp
	mediaplayer.cpp
)

qt4_add_resources(mediaplayer_SRCS
	mediaplayer.qrc
)

qt4_wrap_cpp(mediaplayer_SRCS
	mediaplayer.h
)

qt4_wrap_ui(mediaplayer_SRCS
	settings.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(mediaplayer ${mediaplayer_SRCS})

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

target_link_libraries(mediaplayer
	${QT_QTCORE_LIBRARY_DEBUG}
	${QT_QTGUI_LIBRARY_DEBUG}
	${QT_PHONON_LIBRARY_DEBUG}
)
