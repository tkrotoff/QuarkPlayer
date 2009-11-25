project(ThumbnailView)

add_definitions(-DBUILD_THUMBNAILVIEW)

set(ThumbnailView_SRCS
	ThumbnailListView.cpp
	ThumbnailDirModel.cpp
	ThumbnailManager.cpp
	ThumbnailView.cpp
)

qt4_wrap_cpp(ThumbnailView_SRCS
	ThumbnailDirModel.h
	ThumbnailManager.h
	ThumbnailView.h
)

add_library(ThumbnailView SHARED ${ThumbnailView_SRCS})

target_link_libraries(ThumbnailView
	TkUtil

	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

install(TARGETS ThumbnailView ${INSTALL_TARGETS_DEFAULT_ARGS})
