project(capabilities)

include(${QT_USE_FILE})

set(capabilities_SRCS
	main.cpp
	window.cpp
)

qt4_wrap_cpp(capabilities_SRCS
	window.h
)

add_executable(capabilities ${capabilities_SRCS})

target_link_libraries(capabilities
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_PHONON_LIBRARY}
)

install(TARGETS capabilities DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
