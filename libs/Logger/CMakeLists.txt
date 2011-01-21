project(Logger)

add_definitions(-DBUILD_LOGGER)
add_definitions(-DHACK_ABOUT_TO_QUIT -DHACK_LAYOUT_CHANGED -DHACK_DATA_CHANGED)

set(Logger_SRCS
	Logger.cpp
	LogMessage.cpp
	LogModel.cpp
	LogWindow.cpp
	LoggerOutput.cpp
	ConsoleOutput.cpp
	XmlFileOutput.cpp
)

qt4_wrap_cpp(Logger_SRCS
	Logger.h
	LogModel.h
	LogWindow.h
	LoggerOutput.h
	ConsoleOutput.h
	XmlFileOutput.h
)

add_library(Logger SHARED ${Logger_SRCS})

target_link_libraries(Logger
	TkUtil

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

install(TARGETS Logger ${INSTALL_TARGETS_DEFAULT_ARGS})
