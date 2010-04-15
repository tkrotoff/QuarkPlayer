project(SearchLineEditTest)

set(SearchLineEditTest_SRCS
	SearchLineEditTest.cpp
)

qt4_wrap_cpp(SearchLineEditTest_SRCS
	SearchLineEditTest.h
)

qt4_add_resources(SearchLineEditTest_SRCS
	SearchLineEditTest.qrc
)

add_executable(SearchLineEditTest ${SearchLineEditTest_SRCS})
add_test(SearchLineEditTest SearchLineEditTest)

target_link_libraries(SearchLineEditTest
	TkUtil

	${QT_QTCORE_LIBRARY}
	${QT_QTTEST_LIBRARY}
)

install(TARGETS SearchLineEditTest ${INSTALL_TARGETS_DEFAULT_ARGS})
