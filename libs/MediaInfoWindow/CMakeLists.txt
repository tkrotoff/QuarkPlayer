project(MediaInfoWindow)

add_definitions(-DBUILD_MEDIAINFOWINDOW)

set(MediaInfoWindow_SRCS
	MediaInfoWindow.cpp
	MediaInfoWidget.cpp
)

qt4_wrap_cpp(MediaInfoWindow_SRCS
	MediaInfoWindow.h
	MediaInfoWidget.h
)

qt4_wrap_ui(MediaInfoWindow_SRCS
	MediaInfoWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(MediaInfoWindow SHARED ${MediaInfoWindow_SRCS})

target_link_libraries(MediaInfoWindow
	MediaInfoFetcher
	TkUtil
	ContentFetcher
	WebBrowser
	ThumbnailView
	FileTypes

	${QT_QTCORE_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_PHONON_LIBRARY}
)

# Translations
add_qt_translations(${MediaInfoWindow_SRCS})

install(TARGETS MediaInfoWindow ${INSTALL_TARGETS_DEFAULT_ARGS})
