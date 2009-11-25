project(ContentFetcherTest)

set(ContentFetcherTest_SRCS
	LyricsFetcherTest.cpp
	WikipediaArticleTest.cpp
	AmazonCoverArtTest.cpp
	main.cpp
)

qt4_wrap_cpp(ContentFetcherTest_SRCS
	LyricsFetcherTest.h
	WikipediaArticleTest.h
	AmazonCoverArtTest.h
)

add_executable(ContentFetcherTest ${ContentFetcherTest_SRCS})
add_test(ContentFetcherTest ContentFetcherTest)

target_link_libraries(ContentFetcherTest
	ContentFetcher

	${QT_QTCORE_LIBRARY}
	${QT_QTTEST_LIBRARY}
)

install(TARGETS ContentFetcherTest ${INSTALL_TARGETS_DEFAULT_ARGS})
