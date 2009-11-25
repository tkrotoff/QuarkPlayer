project(QuarkPlayerStyle)

set(QuarkPlayerStyle_SRCS
	QuarkPlayerStyle.cpp
)
qt4_wrap_cpp(QuarkPlayerStyle_SRCS
	QuarkPlayerStyle.h
)
add_library(QuarkPlayerStyle MODULE ${QuarkPlayerStyle_SRCS})
target_link_libraries(QuarkPlayerStyle
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

install(TARGETS QuarkPlayerStyle DESTINATION ${BIN_INSTALL_DIR}/styles)
