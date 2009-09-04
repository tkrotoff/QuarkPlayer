project(lyricsfetchertest)

set(lyricsfetchertest_SRCS
	LyricsFetcherTest.cpp
)

qt4_wrap_cpp(lyricsfetchertest_SRCS
	LyricsFetcherTest.h
)

add_executable(lyricsfetchertest ${lyricsfetchertest_SRCS})
add_test(LyricsFetcherTest lyricsfetchertest)

target_link_libraries(lyricsfetchertest
	contentfetcher

	${QT_QTCORE_LIBRARY}
	${QT_QTTEST_LIBRARY}
)

install(TARGETS lyricsfetchertest ${INSTALL_TARGETS_DEFAULT_ARGS})
