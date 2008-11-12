project(minizip)

include_directories(${ZLIB_INCLUDE_DIR})

#Taken from quazip/quazip.pro
#http://quazip.svn.sourceforge.net/viewvc/quazip/trunk/quazip/quazip/quazip.pro?view=markup
set(minizip_SRCS
	ioapi.c
	unzip.c
	zip.c
)

if (WIN32)
	#Avoid stupid warnings from Visual C++
	add_definitions(
		-D_CRT_SECURE_NO_DEPRECATE
		-D_CRT_NONSTDC_NO_DEPRECATE
	)

	add_definitions(
		-DZLIB_DLL
	)
endif (WIN32)

add_library(minizip STATIC ${minizip_SRCS})

target_link_libraries(minizip
	${ZLIB_LIBRARY}
)

#install(TARGETS minizip DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
