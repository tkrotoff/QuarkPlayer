project(libmplayer)

find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#libmplayer
set(libmplayer_SRCS
	MyProcess.cpp
	MediaData.cpp
	MPlayerVersion.cpp
	MPlayerProcess.cpp
)

qt4_wrap_cpp(libmplayer_SRCS
	MyProcess.h
	MPlayerProcess.h
)

add_library(libmplayer STATIC ${libmplayer_SRCS})
