project(QtIOCompressor)

add_definitions(-DQT_QTIOCOMPRESSOR_EXPORT)

include_directories(${ZLIB_INCLUDE_DIR})

set(QtIOCompressor_SRCS
	qtiocompressor.cpp
)

qt4_wrap_cpp(QtIOCompressor_SRCS
	qtiocompressor.h
)

add_library(QtIOCompressor SHARED ${QtIOCompressor_SRCS})

target_link_libraries(QtIOCompressor
	${QT_QTCORE_LIBRARY}
)

install(TARGETS QtIOCompressor ${INSTALL_TARGETS_DEFAULT_ARGS})
