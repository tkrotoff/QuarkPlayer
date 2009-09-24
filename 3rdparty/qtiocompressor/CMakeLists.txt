project(qtiocompressor)

add_definitions(-DQT_QTIOCOMPRESSOR_EXPORT)

include_directories(${QT_INCLUDE_DIR}/../src/3rdparty/zlib)
include_directories(${QT_QTCORE_INCLUDE_DIR})

set(qtiocompressor_SRCS
	qtiocompressor.cpp
)

qt4_wrap_cpp(qtiocompressor_SRCS
	qtiocompressor.h
)

add_library(qtiocompressor SHARED ${qtiocompressor_SRCS})

target_link_libraries(qtiocompressor
	${QT_QTCORE_LIBRARY}
)

install(TARGETS qtiocompressor ${INSTALL_TARGETS_DEFAULT_ARGS})
