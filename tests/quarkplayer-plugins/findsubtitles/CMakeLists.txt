project(FindSubtitlesTest)

set(FindSubtitlesTest_SRCS
	FindSubtitlesTest.cpp
)

qt4_wrap_cpp(FindSubtitlesTest_SRCS
	FindSubtitlesTest.h
)

add_executable(FindSubtitlesTest ${FindSubtitlesTest_SRCS})

target_link_libraries(FindSubtitlesTest
	findsubtitles

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTTEST_LIBRARY}
)

install(TARGETS FindSubtitlesTest ${INSTALL_TARGETS_DEFAULT_ARGS})
