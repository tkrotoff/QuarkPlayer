project(qttestutil)

set(qttestutil_SRCS
	TestRegistry.cpp
)

add_library(qttestutil STATIC ${qttestutil_SRCS})
