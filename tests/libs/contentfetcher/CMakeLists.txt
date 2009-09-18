project(contentfetchertest)

set(contentfetchertest_SRCS
	LyricsFetcherTest.cpp
	WikipediaArticleTest.cpp
	AmazonCoverArtTest.cpp
	main.cpp
)

qt4_wrap_cpp(contentfetchertest_SRCS
	LyricsFetcherTest.h
	WikipediaArticleTest.h
	AmazonCoverArtTest.h
)

add_executable(contentfetchertest ${contentfetchertest_SRCS})
add_test(contentfetchertest contentfetchertest)

target_link_libraries(contentfetchertest
	contentfetcher

	${QT_QTCORE_LIBRARY}
	${QT_QTTEST_LIBRARY}
)

install(TARGETS contentfetchertest ${INSTALL_TARGETS_DEFAULT_ARGS})
