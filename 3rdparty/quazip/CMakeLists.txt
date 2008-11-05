project(quazip)

include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${MINIZIP_INCLUDE_DIR})

set(quazip_SRCS
	#ioapi.c
	quazip.cpp
	quazipfile.cpp
	quazipnewinfo.cpp
	#unzip.c
	#zip.c
)

qt4_wrap_cpp(findsubtitles_SRCS
	quazipfile.h
)

if (WIN32)
	#Avoid stupid warnings from Visual C++
	add_definitions(
		-D_CRT_SECURE_NO_DEPRECATE
		-D_CRT_NONSTDC_NO_DEPRECATE
	)
endif (WIN32)

add_library(quazip STATIC ${quazip_SRCS})

target_link_libraries(quazip
	${QT_QTCORE_LIBRARY}
	${ZLIB_LIBRARY}
	${MINIZIP_LIBRARY}
)
