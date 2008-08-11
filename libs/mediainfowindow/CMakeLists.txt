project(mediainfowindow)

add_definitions(-DBUILD_MEDIAINFOWINDOW)

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
	${QT_QTCORE_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_PHONON_LIBRARY}
)

install(TARGETS mediainfowindow DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
