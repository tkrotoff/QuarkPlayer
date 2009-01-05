find_program(DEBUILD_EXECUTABLE debuild)

if (DEBUILD_EXECUTABLE)

	message(STATUS "Found debuild: ${DEBUILD_EXECUTABLE}")

	configure_file(control.in ${CMAKE_CURRENT_SOURCE_DIR}/control @ONLY)

	add_custom_target(deb
		${DEBUILD_EXECUTABLE} -i -us -uc -b
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)

else (DEBUILD_EXECUTABLE)
	message(STATUS "Could not find debuild")
endif (DEBUILD_EXECUTABLE)
