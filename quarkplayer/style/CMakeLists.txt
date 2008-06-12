project(quarkplayerstyle)

set(quarkplayerstyle_SRCS
	QuarkPlayerStyle.cpp
)
qt4_wrap_cpp(quarkplayerstyle_SRCS
	QuarkPlayerStyle.h
)
add_library(quarkplayerstyle MODULE ${quarkplayerstyle_SRCS})
target_link_libraries(quarkplayerstyle
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

install(TARGETS quarkplayerstyle DESTINATION ${BIN_INSTALL_DIR}/styles)
