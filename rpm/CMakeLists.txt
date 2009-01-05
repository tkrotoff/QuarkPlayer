find_program(RPMBUILD_EXECUTABLE rpmbuild)

if (RPMBUILD_EXECUTABLE)

	message(STATUS "Found rpmbuild: ${RPMBUILD_EXECUTABLE}")

	#Trick: define our own topdir so there is no need to be root to create
	#the package
	set(RPM_TOP_DIR ${CMAKE_BINARY_DIR}/rpmbuild)

	#Trick: BUILD directory is in fact our source code directory
	set(RPM_BUILD_DIR ${CMAKE_SOURCE_DIR})

	#Define our own RPM filename so that we can add special informations to the name
	set(RPM_FILENAME ${QUARKPLAYER_PACKAGE_FILE}.rpm)

	configure_file(quarkplayer.spec.in ${CMAKE_BINARY_DIR}/quarkplayer.spec @ONLY)

	# Prepare RPM build tree
	file(MAKE_DIRECTORY ${RPM_TOP_DIR})
	file(MAKE_DIRECTORY ${RPM_TOP_DIR}/BUILD)
	file(MAKE_DIRECTORY ${RPM_TOP_DIR}/RPMS)
	file(MAKE_DIRECTORY ${RPM_TOP_DIR}/SOURCES)
	file(MAKE_DIRECTORY ${RPM_TOP_DIR}/SPECS)
	file(MAKE_DIRECTORY ${RPM_TOP_DIR}/SRPMS)

	add_custom_target(rpm
		${RPMBUILD_EXECUTABLE} -bb ${CMAKE_BINARY_DIR}/quarkplayer.spec
	)

else (RPMBUILD_EXECUTABLE)
	message(STATUS "Could not find rpmbuild")
endif (RPMBUILD_EXECUTABLE)
