find_file(DEBUILD_PROGRAM
	NAMES
		debuild
	PATHS
		/usr/bin
)

if (DEBUILD_PROGRAM)
	message(STATUS "Found debuild: ${DEBUILD_PROGRAM}")
else (DEBUILD_PROGRAM)
	message(FATAL_ERROR "Could not find debuild")
endif (DEBUILD_PROGRAM)

configure_file(control.in ${CMAKE_CURRENT_SOURCE_DIR}/control @ONLY)

add_custom_target(deb
	${DEBUILD_PROGRAM} -S
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
