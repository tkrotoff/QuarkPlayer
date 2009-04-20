project(mediainfowindow)

if (TAGLIB)
	# This is because of the pretty strange taglib structure
	include_directories(${CMAKE_SOURCE_DIR}/3rdparty/taglib)
endif (TAGLIB)

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_definitions(-DBUILD_MEDIAINFOWINDOW)

# Otherwise QString::fromStdWString() is not defined
# This happens when compiling using the KDE4 libraries
remove_definitions(-DQT_NO_STL)
remove_definitions(-DQT_NO_STL_WCHAR)

set(mediainfowindow_SRCS
	MediaInfoWindow.cpp
	MediaInfoFetcher.cpp
	MediaInfo.cpp
)

if (MSVC)
	if (MSVC_VERSION GREATER 1399)
		# If using Visual C++ 2005 (MSVC80) and greater (MSVC_VERSION=1400)
		# Do not treat wchar_t as built-in type otherwise QString::fromStdWString()
		# and QString::toStdWString() get unresolved
		set_source_files_properties(MediaInfoFetcher.cpp
			PROPERTIES COMPILE_FLAGS "/Zc:wchar_t-"
		)
	endif (MSVC_VERSION GREATER 1399)
endif (MSVC)

qt4_wrap_cpp(mediainfowindow_SRCS
	MediaInfoWindow.h
	MediaInfoFetcher.h
)

qt4_wrap_ui(mediainfowindow_SRCS
	MediaInfoWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(mediainfowindow SHARED ${mediainfowindow_SRCS})

target_link_libraries(mediainfowindow
	tkutil
	contentfetcher
	webbrowser
	thumbnailview
	filetypes
	${QT_QTCORE_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_PHONON_LIBRARY}
)

if (UNIX)
	# Otherwise dlopen, dlsym, dlclose are unresolved
	# This happens when compiling using the KDE4 libraries
	target_link_libraries(mediainfowindow dl)
endif (UNIX)

if (TAGLIB)
	target_link_libraries(mediainfowindow tag)
endif (TAGLIB)

if (MEDIAINFOLIB)
	target_link_libraries(mediainfowindow mediainfo)
endif (MEDIAINFOLIB)

# Translations
add_qt_translations(${mediainfowindow_SRCS})

install(TARGETS mediainfowindow DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
