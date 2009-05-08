project(modeltest)

set(modeltest_SRCS
	modeltest.cpp
)

qt4_wrap_cpp(modeltest_SRCS
	modeltest.h
)

add_library(modeltest STATIC ${modeltest_SRCS})

target_link_libraries(modeltest
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)
