project(qtsingleapplication)

add_definitions(-DQT_QTSINGLEAPPLICATION_EXPORT)

set(qtsingleapplication_SRCS
	qtsinglecoreapplication.cpp
	qtsingleapplication.cpp
	qtlocalpeer.cpp
)

qt4_wrap_cpp(qtsingleapplication_SRCS
	qtsinglecoreapplication.h
	qtsingleapplication.h
	qtlocalpeer.h
)

add_library(qtsingleapplication SHARED ${qtsingleapplication_SRCS})

target_link_libraries(qtsingleapplication
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
)

install(TARGETS qtsingleapplication DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
