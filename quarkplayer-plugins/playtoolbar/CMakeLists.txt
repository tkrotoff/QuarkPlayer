project(playtoolbar)

set(playtoolbar_SRCS
	PlayToolBar.cpp
)

qt4_wrap_cpp(playtoolbar_SRCS
	PlayToolBar.h
)

add_library(playtoolbar MODULE ${playtoolbar_SRCS})

target_link_libraries(playtoolbar
	quarkplayercore
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

#Translations
add_qt_translations(${playtoolbar_SRCS})

install(TARGETS playtoolbar DESTINATION ${BIN_INSTALL_DIR}/plugins)
