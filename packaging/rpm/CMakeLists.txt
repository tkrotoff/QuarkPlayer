find_program(RPMBUILD_EXECUTABLE rpmbuild)

if (RPMBUILD_EXECUTABLE)

	message(STATUS "Found rpmbuild: ${RPMBUILD_EXECUTABLE}")

	configure_file(quarkplayer.spec.in ${CMAKE_CURRENT_BINARY_DIR}/quarkplayer.spec @ONLY)

	add_custom_target(rpm
		${RPMBUILD_EXECUTABLE} -bb ${CMAKE_CURRENT_BINARY_DIR}/quarkplayer.spec
	)

else (RPMBUILD_EXECUTABLE)
	message(STATUS "Could not find rpmbuild")
endif (RPMBUILD_EXECUTABLE)
