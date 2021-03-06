find_program(FAKEROOT_EXECUTABLE fakeroot)
find_program(DH_EXECUTABLE dh)

if (FAKEROOT_EXECUTABLE AND DH_EXECUTABLE)

	message(STATUS "Found fakeroot: ${FAKEROOT_EXECUTABLE}")
	message(STATUS "Found dh: ${DH_EXECUTABLE}")

	# Where the package will be built
	# By default it is $(CURDIR)/debian/quarkplayer but we want it to be
	# inside CMAKE_BINARY_DIR, more coherent with CMake philosophy
	set(DEB_BUILD_DIR ${CMAKE_BINARY_DIR}/debian/quarkplayer)

	configure_file(control.in ${CMAKE_CURRENT_SOURCE_DIR}/control @ONLY)
	configure_file(rules.in ${CMAKE_CURRENT_SOURCE_DIR}/rules @ONLY)
	configure_file(changelog.in ${CMAKE_CURRENT_SOURCE_DIR}/changelog @ONLY)
	configure_file(docs.in ${CMAKE_CURRENT_SOURCE_DIR}/docs @ONLY)

	# -b : binary-only, do not build source
	# -us : unsigned source
	# -uc : unsigned changes
	add_custom_target(deb
		# Fails on dpkg-genchanges :/
		#debuild -i -us -uc -b
		#dpkg-buildpackage -rfakeroot -i -us -uc -b
		${FAKEROOT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/rules binary
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/packaging/
	)
endif (FAKEROOT_EXECUTABLE AND DH_EXECUTABLE)

if (NOT FAKEROOT_EXECUTABLE)
	message(STATUS "Could NOT find fakeroot")
endif (NOT FAKEROOT_EXECUTABLE)

if (NOT DH_EXECUTABLE)
	message(STATUS "Could NOT find dh")
endif (NOT DH_EXECUTABLE)
