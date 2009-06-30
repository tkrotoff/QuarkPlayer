project(contentfetcher)

add_definitions(-DBUILD_CONTENTFETCHER)

set(contentfetcher_SRCS
	ContentFetcher.cpp
	AmazonCoverArt.cpp
	LyricsFetcher.cpp
)

qt4_wrap_cpp(contentfetcher_SRCS
	ContentFetcher.h
	AmazonCoverArt.h
	LyricsFetcher.h
)

add_library(contentfetcher SHARED ${contentfetcher_SRCS})

target_link_libraries(contentfetcher
	hmac

	${QT_QTCORE_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

install(TARGETS contentfetcher ${INSTALL_TARGETS_DEFAULT_ARGS})
