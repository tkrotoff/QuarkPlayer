project(FindSubtitlesTest)

set(FindSubtitlesTest_SRCS
	FindSubtitlesTest.cpp
	ZipFileTest.cpp
	main.cpp
)

qt4_wrap_cpp(FindSubtitlesTest_SRCS
	FindSubtitlesTest.h
	ZipFileTest.h
)

qt4_add_resources(FindSubtitlesTest_SRCS
	zipfiles.qrc
)

add_executable(FindSubtitlesTest ${FindSubtitlesTest_SRCS})

target_link_libraries(FindSubtitlesTest
	FindSubtitles

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTTEST_LIBRARY}
)

install(TARGETS FindSubtitlesTest ${INSTALL_TARGETS_DEFAULT_ARGS})
