find_file(RPMBUILD_PROGRAM
	NAMES
		rpmbuild
	PATHS
		/usr/bin
)

if (RPMBUILD_PROGRAM)
	message(STATUS "Found rpmbuild: ${RPMBUILD_PROGRAM}")
else (RPMBUILD_PROGRAM)
	message(FATAL_ERROR "Could not find rpmbuild")
endif (RPMBUILD_PROGRAM)

configure_file(quarkplayer.spec.in ${CMAKE_BINARY_DIR}/quarkplayer.spec @ONLY)

#Trick: define our own topdir so there is no need to be root to create
#the package
set(TOPDIR ${CMAKE_SOURCE_DIR}/rpm/build)

#Define our own RPM filename so that we can add special informations to the name
set(RPMFILENAME ${QUARKPLAYER_PACKAGE_FILE}.rpm)

add_custom_target(rpm
	${RPMBUILD_PROGRAM}
	--define="_topdir ${TOPDIR}"
	--define="_rpmfilename ${CMAKE_SOURCE_DIR}/${RPMFILENAME}"
	-bb ${CMAKE_BINARY_DIR}/quarkplayer.spec
)
