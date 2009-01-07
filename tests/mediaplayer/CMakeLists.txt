project(mediaplayer)

include(${QT_USE_FILE})

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

target_link_libraries(mediaplayer
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_PHONON_LIBRARY}
)

install(TARGETS mediaplayer DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
