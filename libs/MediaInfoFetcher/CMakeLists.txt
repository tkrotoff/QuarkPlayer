project(MediaInfoFetcher)

if (TAGLIB)
	include_directories(${TAGLIB_INCLUDE_DIRS})
	add_definitions(${TAGLIB_DEFINITIONS})
	add_definitions(-DTAGLIB)
endif (TAGLIB)

if (MEDIAINFOLIB)
	include_directories(${MEDIAINFOLIB_INCLUDE_DIRS})
	add_definitions(${MEDIAINFOLIB_DEFINITIONS})
	add_definitions(-DMEDIAINFOLIB)
endif (MEDIAINFOLIB)

add_definitions(-DBUILD_MEDIAINFOFETCHER)

# Otherwise QString::fromStdWString() is not defined
# This happens when compiling using the KDE4 libraries
remove_definitions(-DQT_NO_STL)
remove_definitions(-DQT_NO_STL_WCHAR)

set(MediaInfoFetcher_SRCS
	MediaInfoFetcher.cpp
	MediaInfo.cpp
	MetaDataWriter.cpp
)

if (MSVC)
	if (MSVC_VERSION GREATER 1399)
		# If using Visual C++ 2005 (MSVC80, MSVC_VERSION=1400) and greater
		# Do not treat wchar_t as built-in type otherwise QString::fromStdWString()
		# and QString::toStdWString() get unresolved
		# See http://www.qtforum.org/post/69786/problem-using-qstring-fromstdwstring.html
		# See http://www.qtcentre.org/forum/f-qt-programming-2/t-wchar-to-qstring-giving-error-in-vs2005-4625.html
		# See http://lists.trolltech.com/pipermail/qt-interest/2009-February/002259.html
		# See http://www.qtcentre.org/forum/f-qt-programming-2/t-finding-window-linker-error-12478.html
		# See http://thread.gmane.org/gmane.comp.lib.qt.general/8486
		# /Zc:wchar_t- is not recognized under Visual C++ .NET 2003 (MSVC71)
		set_source_files_properties(
			MediaInfoFetcher.cpp
			MetaDataWriter.cpp
			PROPERTIES COMPILE_FLAGS "/Zc:wchar_t-"
		)
	endif (MSVC_VERSION GREATER 1399)
endif (MSVC)

qt4_wrap_cpp(MediaInfoFetcher_SRCS
	MediaInfoFetcher.h
)

add_library(MediaInfoFetcher SHARED ${MediaInfoFetcher_SRCS})

target_link_libraries(MediaInfoFetcher
	TkUtil
	FileTypes

	${QT_QTCORE_LIBRARY}
	${QT_PHONON_LIBRARY}
)

if (UNIX)
	# Otherwise dlopen, dlsym, dlclose are unresolved
	# This happens when compiling using the KDE4 libraries
	target_link_libraries(MediaInfoFetcher dl)
endif (UNIX)

if (TAGLIB)
	target_link_libraries(MediaInfoFetcher ${TAGLIB_LIBRARIES})
endif (TAGLIB)

if (MEDIAINFOLIB)
	target_link_libraries(MediaInfoFetcher ${MEDIAINFOLIB_LIBRARIES})
endif (MEDIAINFOLIB)

install(TARGETS MediaInfoFetcher ${INSTALL_TARGETS_DEFAULT_ARGS})
