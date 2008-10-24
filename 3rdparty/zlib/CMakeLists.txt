project(zlib)

#Taken from the root Makefile
set(zlib_SRCS
	adler32.c
	compress.c
	crc32.c
	deflate.c
	#example.c
	gzio.c
	inffast.c
	inflate.c
	infback.c
	inftrees.c
	minigzip.c
	trees.c
	uncompr.c
	zutil.c
)

if (WIN32)
	#Taken from win32/Makefile.msc
	list(APPEND quarkplayer_SRCS
		win32/zlib1.rc
		#win32/zlib.def
	)

	#Avoid stupid warnings from Visual C++
	add_definitions(
		-D_CRT_SECURE_NO_DEPRECATE
		-D_CRT_NONSTDC_NO_DEPRECATE
	)

	add_definitions(
		#-DZLIB_WINAPI
		-DZLIB_DLL
	)
endif (WIN32)

add_library(zlib SHARED ${zlib_SRCS})

install(TARGETS zlib DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
