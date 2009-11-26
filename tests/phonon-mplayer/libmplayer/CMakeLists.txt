project(SimpleMPlayer)

set(SimpleMPlayer_SRCS
	main.cpp
	SimpleMPlayer.cpp
)

qt4_wrap_cpp(SimpleMPlayer_SRCS
	SimpleMPlayer.h
)

qt4_wrap_ui(SimpleMPlayer_SRCS
	SimpleMPlayer.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(SimpleMPlayer ${SimpleMPlayer_SRCS})

target_link_libraries(SimpleMPlayer
	libmplayer

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTMAIN_LIBRARY}
)

install(TARGETS SimpleMPlayer DESTINATION ${BIN_INSTALL_DIR})
