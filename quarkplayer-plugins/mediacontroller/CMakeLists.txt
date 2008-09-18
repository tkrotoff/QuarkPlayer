project(mediacontroller)

set(mediacontroller_SRCS
	MediaController.cpp
	MediaControllerToolBar.cpp
)

qt4_wrap_cpp(mediacontroller_SRCS
	MediaController.h
	MediaControllerToolBar.h
)

add_library(mediacontroller MODULE ${mediacontroller_SRCS})

target_link_libraries(mediacontroller
	quarkplayercore
	filetypes
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

#Translations
add_qt_translations(${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS mediacontroller DESTINATION ${BIN_INSTALL_DIR}/plugins)
