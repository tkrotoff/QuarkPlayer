project(contentfetcher)

add_definitions(-DBUILD_CONTENTFETCHER)

set(contentfetcher_SRCS
	ContentFetcher.cpp
	AmazonCoverArt.cpp
	LyricsFetcher.cpp
	WikipediaArticle.cpp
)

qt4_wrap_cpp(contentfetcher_SRCS
	ContentFetcher.h
	AmazonCoverArt.h
	LyricsFetcher.h
	WikipediaArticle.h
)

add_library(contentfetcher SHARED ${contentfetcher_SRCS})

target_link_libraries(contentfetcher
	${QT_QTCORE_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

install(TARGETS contentfetcher ${INSTALL_TARGETS_DEFAULT_ARGS})
