project(capabilities)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDE_DIR})

set(capabilities_SRCS
	main.cpp
	window.cpp
)

qt4_wrap_cpp(capabilities_SRCS
	window.h
)

add_executable(capabilities ${capabilities_SRCS})

if (UNIX)
	set(QT_PHONON_LIBRARY_DEBUG ${QT_LIBRARY_DIR}/libphonon.so.4.1.0.debug)
endif (UNIX)

if (WIN32)
	set(QT_PHONON_LIBRARY_DEBUG ${QT_LIBRARY_DIR}/phonond4.lib)
endif (WIN32)

target_link_libraries(capabilities
	${QT_QTCORE_LIBRARY_DEBUG}
	${QT_QTGUI_LIBRARY_DEBUG}
	${QT_PHONON_LIBRARY_DEBUG}
)
