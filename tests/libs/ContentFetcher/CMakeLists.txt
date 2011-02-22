project(ContentFetcherTests)

macro(add_my_test name)
	set(SRCS ${ARGN})

	add_executable(${name} ${SRCS})

	add_test(${name} ${name})

	target_link_libraries(${name}
		ContentFetcher

		${QT_QTCORE_LIBRARY}
		${QT_QTTEST_LIBRARY}
	)
	install(TARGETS ${name} ${INSTALL_TARGETS_DEFAULT_ARGS})
endmacro(add_my_test name)


set(LyricsFetcherTest_SRCS LyricsFetcherTest.cpp)
qt4_wrap_cpp(LyricsFetcherTest_SRCS LyricsFetcherTest.h)
add_my_test(LyricsFetcherTest ${LyricsFetcherTest_SRCS})


set(WikipediaArticleTest_SRCS WikipediaArticleTest.cpp)
qt4_wrap_cpp(WikipediaArticleTest_SRCS WikipediaArticleTest.h)
add_my_test(WikipediaArticleTest ${WikipediaArticleTest_SRCS})


set(AmazonCoverArtTest_SRCS AmazonCoverArtTest.cpp)
qt4_wrap_cpp(AmazonCoverArtTest_SRCS AmazonCoverArtTest.h)
add_my_test(AmazonCoverArtTest ${AmazonCoverArtTest_SRCS})
