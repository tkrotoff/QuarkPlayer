project(findsubtitles)

include_directories(${ZLIB_INCLUDE_DIR})

if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

set(findsubtitles_SRCS
	FindSubtitles.cpp
	FindSubtitlesWindow.cpp
	OpenSubtitlesParser.cpp
	FileChooserWindow.cpp
)

qt4_wrap_cpp(findsubtitles_SRCS
	FindSubtitles.h
	FindSubtitlesWindow.h
	FileChooserWindow.h
)

qt4_wrap_ui(findsubtitles_SRCS
	FindSubtitlesWindow.ui
	FileChooserWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (STATICPLUGINS)
	add_library(findsubtitles STATIC ${findsubtitles_SRCS})
else (STATICPLUGINS)
	add_library(findsubtitles MODULE ${findsubtitles_SRCS})
endif (STATICPLUGINS)

target_link_libraries(findsubtitles
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTXML_LIBRARY}
	tkutil
	quazip
	quarkplayercore
)

# Translations
add_qt_translations(${findsubtitles_SRCS})

if (NOT STATICPLUGINS)
	install(TARGETS findsubtitles ${INSTALL_TARGETS_PLUGINS_DEFAULT_ARGS})
endif (NOT STATICPLUGINS)
