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

if (KDE4_FOUND)
	target_link_libraries(capabilities
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${QT_PHONON_LIBRARY}
	)
else (KDE4_FOUND)
	target_link_libraries(capabilities
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_PHONON_LIBRARY}
	)
endif (KDE4_FOUND)

install(TARGETS capabilities DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
