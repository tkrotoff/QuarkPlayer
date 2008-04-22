project(libmplayer)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(libmplayer_SRCS
	MyProcess.cpp
	MediaData.cpp
	MediaSettings.cpp
	MPlayerVersion.cpp
	MPlayerProcess.cpp
	MPlayerLoader.cpp
	MPlayerWindow.cpp
)

qt4_wrap_cpp(libmplayer_SRCS
	MyProcess.h
	MPlayerProcess.h
	MPlayerWindow.h
)

add_library(libmplayer STATIC ${libmplayer_SRCS})
