project(FileTypes)

add_definitions(-DBUILD_FILETYPES)

set(FileTypes_SRCS
	FileTypes.cpp
)

add_library(FileTypes SHARED ${FileTypes_SRCS})

target_link_libraries(FileTypes
	${QT_QTCORE_LIBRARY}
)

install(TARGETS FileTypes ${INSTALL_TARGETS_DEFAULT_ARGS})
