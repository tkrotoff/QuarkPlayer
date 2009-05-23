project(zenlib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

if (NOT MINGW)
	# MinGW STL does not support wide characters (wchar_t)
	# see http://www.mingw.org/wiki/wide_characters
	add_definitions(-DUNICODE)
endif (NOT MINGW)

set(zenlib_SRCS
	Conf.cpp
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
	Format/Html/Html_Handler.cpp
	Format/Html/Html_Request.cpp
	Format/Http/Http_Cookies.cpp
	Format/Http/Http_Handler.cpp
	Format/Http/Http_Request.cpp
	Format/Http/Http_Utils.cpp
)

add_library(zenlib STATIC ${zenlib_SRCS})
