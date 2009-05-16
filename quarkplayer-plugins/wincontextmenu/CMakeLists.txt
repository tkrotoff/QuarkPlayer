project(quarkplayercontextmenu)

add_definitions(-DUNICODE)

set(quarkplayercontextmenu_SRCS
	WinContextMenu.cpp
	WinContextMenu.def
	WinContextMenu.rc
)

add_library(quarkplayercontextmenu SHARED ${quarkplayercontextmenu_SRCS})

install(TARGETS quarkplayercontextmenu ${INSTALL_TARGETS_DEFAULT_ARGS})
