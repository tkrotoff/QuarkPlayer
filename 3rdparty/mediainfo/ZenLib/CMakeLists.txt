project(ZenLib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set(ZenLib_SRCS
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
	TinyXml/tinyxml.cpp
	TinyXml/tinyxmlerror.cpp
	TinyXml/tinyxmlparser.cpp
)

add_library(ZenLib STATIC ${ZenLib_SRCS})
