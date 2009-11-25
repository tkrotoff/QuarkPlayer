project(QtSingleApplication)

add_definitions(-DQT_QTSINGLEAPPLICATION_EXPORT)

set(QtSingleApplication_SRCS
	qtsinglecoreapplication.cpp
	qtsingleapplication.cpp
	qtlocalpeer.cpp
)

qt4_wrap_cpp(QtSingleApplication_SRCS
	qtsinglecoreapplication.h
	qtsingleapplication.h
	qtlocalpeer.h
)

add_library(QtSingleApplication SHARED ${QtSingleApplication_SRCS})

target_link_libraries(QtSingleApplication
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
)

install(TARGETS QtSingleApplication ${INSTALL_TARGETS_DEFAULT_ARGS})
