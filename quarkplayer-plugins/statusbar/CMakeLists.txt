project(statusbar)

set(statusbar_SRCS
	StatusBar.cpp
)

qt4_wrap_cpp(statusbar_SRCS
	StatusBar.h
)

add_library(statusbar MODULE ${statusbar_SRCS})

target_link_libraries(statusbar
	quarkplayercore
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

#Translations
add_qt_translations(${statusbar_SRCS})

install(TARGETS statusbar DESTINATION ${BIN_INSTALL_DIR}/plugins)
