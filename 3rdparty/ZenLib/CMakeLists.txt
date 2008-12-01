project(zenlib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

set(zenlib_SRCS
	Conf.cpp
	ConvertUTF.c
	CriticalSection.cpp
	Dir.cpp
	File.cpp
	FileName.cpp
	HTTP_Client.cpp
	InfoMap.cpp
	int128s.cpp
	int128u.cpp
	MemoryDebug.cpp
	OS_Utils.cpp
	Translation.cpp
	Thread.cpp
	Utils.cpp
	Ztring.cpp
	ZtringList.cpp
	ZtringListList.cpp
	ZtringListListF.cpp
)

add_library(zenlib STATIC ${zenlib_SRCS})
