project(QuarkPlayerContextMenu)

set(QuarkPlayerContextMenu_SRCS
	WinContextMenu.cpp
	WinContextMenu.def
	WinContextMenu.rc
)

add_library(QuarkPlayerContextMenu SHARED ${QuarkPlayerContextMenu_SRCS})

install(TARGETS QuarkPlayerContextMenu ${INSTALL_TARGETS_DEFAULT_ARGS})
