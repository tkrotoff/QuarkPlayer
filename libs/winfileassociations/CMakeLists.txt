project(winfileassociations)

add_definitions(-DBUILD_WINFILEASSOCIATIONS)

set(winfileassociations_SRCS
	WinFileAssociations.cpp
)

add_library(winfileassociations SHARED ${winfileassociations_SRCS})

target_link_libraries(winfileassociations
	${QT_QTCORE_LIBRARY}
)

install(TARGETS winfileassociations DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
