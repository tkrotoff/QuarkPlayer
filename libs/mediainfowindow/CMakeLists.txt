project(mediainfowindow)

if (TAGLIB)
	#This is because of the pretty strange taglib structure
	include_directories(${CMAKE_SOURCE_DIR}/3rdparty/taglib)
endif (TAGLIB)

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_definitions(-DBUILD_MEDIAINFOWINDOW)

if (MEDIAINFO)
	#MediaInfo is compiled with Unicode support on
	#Do not treat wchar_t as built-in type otherwise QString::fromStdWString()
	#and QString::toStdWString() get unresolved
	add_definitions(
		-DUNICODE
		/Zc:wchar_t-
	)
endif (MEDIAINFO)

set(mediainfowindow_SRCS
	MediaInfoWindow.cpp
	MediaInfoFetcher.cpp
)

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

if (TAGLIB)
	target_link_libraries(mediainfowindow tag)
endif (TAGLIB)

if (MEDIAINFO)
	target_link_libraries(mediainfowindow mediainfo)
endif (MEDIAINFO)

#Translations
add_qt_translations(${mediainfowindow_SRCS})

install(TARGETS mediainfowindow DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
