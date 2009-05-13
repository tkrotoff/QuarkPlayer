project(quarkplayercontextmenu)

set(quarkplayercontextmenu_SRCS
	QuarkPlayerContextMenu.cpp
	QuarkPlayerContextMenu.def
	QuarkPlayerContextMenu.rc
)

add_library(quarkplayercontextmenu SHARED ${quarkplayercontextmenu_SRCS})

install(TARGETS quarkplayercontextmenu ${INSTALL_TARGETS_DEFAULT_ARGS})
