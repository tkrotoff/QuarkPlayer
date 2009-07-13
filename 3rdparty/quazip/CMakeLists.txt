project(quazip)

include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${MINIZIP_INCLUDE_DIR})

set(quazip_SRCS
	quazip.cpp
	quazipfile.cpp
	quazipnewinfo.cpp
)

qt4_wrap_cpp(quazip_SRCS
	quazipfile.h
)

add_library(quazip STATIC ${quazip_SRCS})

target_link_libraries(quazip
	${ZLIB_LIBRARY}
	${MINIZIP_LIBRARY}

	${QT_QTCORE_LIBRARY}
)
