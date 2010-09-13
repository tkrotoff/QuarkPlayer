project(Logger)

add_definitions(-DBUILD_LOGGER)
add_definitions(-DHACK_ABOUT_TO_QUIT -DHACK_LAYOUT_CHANGED)

set(Logger_SRCS
	LogMessageHandler.cpp
	LogMessage.cpp
	LogModel.cpp
	LogWindow.cpp
)

qt4_wrap_cpp(Logger_SRCS
	LogMessageHandler.h
	LogModel.h
	LogWindow.h
)

add_library(Logger SHARED ${Logger_SRCS})

target_link_libraries(Logger
	TkUtil

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

install(TARGETS Logger ${INSTALL_TARGETS_DEFAULT_ARGS})
