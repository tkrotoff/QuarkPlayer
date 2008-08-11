project(profiling)

set(profiling_SRCS
	TimeElapsed.cpp
)

add_library(profiling STATIC ${profiling_SRCS})

target_link_libraries(profiling
	${QT_QTCORE_LIBRARY}
)
