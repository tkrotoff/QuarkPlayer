project(filetypes)

add_definitions(-DBUILD_FILETYPES)

set(filetypes_SRCS
	FileTypes.cpp
)

add_library(filetypes SHARED ${filetypes_SRCS})

target_link_libraries(filetypes
	${QT_QTCORE_LIBRARY}
)

install(TARGETS filetypes DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
