project(simplemplayer)

set(simplemplayer_SRCS
	main.cpp
	SimpleMPlayer.cpp
)

qt4_wrap_cpp(simplemplayer_SRCS
	SimpleMPlayer.h
)

qt4_wrap_ui(simplemplayer_SRCS
	SimpleMPlayer.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(simplemplayer ${simplemplayer_SRCS})

target_link_libraries(simplemplayer
	libmplayer

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTMAIN_LIBRARY}
)

install(TARGETS simplemplayer DESTINATION ${BIN_INSTALL_DIR})
