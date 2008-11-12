project(findsubtitles)

include_directories(${ZLIB_INCLUDE_DIR})

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

add_library(findsubtitles MODULE ${findsubtitles_SRCS})

target_link_libraries(findsubtitles
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTXML_LIBRARY}
	tkutil
	quazip
	quarkplayercore
)

#Translations
add_qt_translations(${findsubtitles_SRCS})

install(TARGETS findsubtitles DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES findsubtitles.pluginspec DESTINATION ${PLUGIN_INSTALL_DIR})
