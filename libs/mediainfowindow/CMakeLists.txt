project(mediainfowindow)

add_definitions(-DBUILD_MEDIAINFOWINDOW)

set(mediainfowindow_SRCS
	MediaInfoWindow.cpp
)

qt4_wrap_cpp(mediainfowindow_SRCS
	MediaInfoWindow.h
)

qt4_wrap_ui(mediainfowindow_SRCS
	MediaInfoWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(mediainfowindow SHARED ${mediainfowindow_SRCS})

target_link_libraries(mediainfowindow
	mediainfofetcher
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

# Translations
add_qt_translations(${mediainfowindow_SRCS})

install(TARGETS mediainfowindow ${INSTALL_TARGETS_DEFAULT_ARGS})
