project(winshellcontextmenu)

set(winshellcontextmenu_SRCS
	WinShellContextMenu.cpp
	WinShellContextMenu.def
	WinShellContextMenu.rc
)

add_library(winshellcontextmenu SHARED ${winshellcontextmenu_SRCS})

install(TARGETS winshellcontextmenu RUNTIME DESTINATION ${BIN_INSTALL_DIR})
