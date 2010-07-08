project(FindSubtitles)

include_directories(${QTIOCOMPRESSOR_INCLUDE_DIRS})
add_definitions(${QTIOCOMPRESSOR_DEFINITIONS})

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(FindSubtitles_SRCS
	FindSubtitles.cpp
	FindSubtitlesWindow.cpp
	OpenSubtitlesParser.cpp
	FileChooserWindow.cpp
	ZipFile.cpp
	OpenSubtitlesDownload.cpp
)

qt4_wrap_cpp(FindSubtitles_SRCS
	FindSubtitles.h
	FindSubtitlesWindow.h
	FileChooserWindow.h
	OpenSubtitlesDownload.h
)

qt4_wrap_ui(FindSubtitles_SRCS
	FindSubtitlesWindow.ui
	FileChooserWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (STATICPLUGINS)
	add_library(FindSubtitles STATIC ${FindSubtitles_SRCS})
else (STATICPLUGINS)
	add_library(FindSubtitles MODULE ${FindSubtitles_SRCS})
endif (STATICPLUGINS)

target_link_libraries(FindSubtitles
	QuarkPlayerCore
	MainWindow
	TkUtil

	${QTIOCOMPRESSOR_LIBRARIES}

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTXML_LIBRARY}
)

# Translations
add_qt_translations(${FindSubtitles_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS FindSubtitles ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
