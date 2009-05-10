if (WIN32)
	# NSIS installer script
	add_subdirectory(nsis)
endif (WIN32)

if (LINUX)
	# Debian installer script
	add_subdirectory(debian)

	# RPM installer script
	add_subdirectory(rpm)
endif (LINUX)
