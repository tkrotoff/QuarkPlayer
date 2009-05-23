project(zlib)

# Taken from the root Makefile
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
	#minigzip.c
	trees.c
	uncompr.c
	zutil.c
)

if (WIN32)
	#Taken from win32/Makefile.msc
	list(APPEND quarkplayer_SRCS
		win32/zlib1.rc
	)

	#Avoid stupid warnings from Visual C++
	add_definitions(
		-D_CRT_SECURE_NO_DEPRECATE
		-D_CRT_NONSTDC_NO_DEPRECATE
	)

	add_definitions(
		-DZLIB_DLL
	)

	if (MSVC_VERSION GREATER 1310)
		# If Visual C++ > .NET 2003 7.1
		# otherwise vsnprintf is not defined
		add_definitions(-DHAVE_VSNPRINTF)
	endif (MSVC_VERSION GREATER 1310)
endif (WIN32)

add_library(zlib SHARED ${zlib_SRCS})

install(TARGETS zlib ${INSTALL_TARGETS_DEFAULT_ARGS})
