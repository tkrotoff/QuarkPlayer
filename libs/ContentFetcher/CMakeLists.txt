project(ContentFetcher)

add_definitions(-DBUILD_CONTENTFETCHER)

set(ContentFetcher_SRCS
	ContentFetcher.cpp
	AmazonCoverArt.cpp
	LyricsFetcher.cpp
	WikipediaArticle.cpp
)

qt4_wrap_cpp(ContentFetcher_SRCS
	ContentFetcher.h
	AmazonCoverArt.h
	LyricsFetcher.h
	WikipediaArticle.h
)

add_library(ContentFetcher SHARED ${ContentFetcher_SRCS})

target_link_libraries(ContentFetcher
	hmac

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTXML_LIBRARY}
)

install(TARGETS ContentFetcher ${INSTALL_TARGETS_DEFAULT_ARGS})
