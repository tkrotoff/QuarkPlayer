project(thumbnailview)

add_definitions(-DBUILD_THUMBNAILVIEW)

set(thumbnailview_SRCS
	ThumbnailListView.cpp
	ThumbnailDirModel.cpp
	ThumbnailManager.cpp
	ThumbnailView.cpp
)

qt4_wrap_cpp(thumbnailview_SRCS
	ThumbnailDirModel.h
	ThumbnailManager.h
	ThumbnailView.h
)

add_library(thumbnailview SHARED ${thumbnailview_SRCS})

target_link_libraries(thumbnailview
	tkutil
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

install(TARGETS thumbnailview ${INSTALL_TARGETS_DEFAULT_ARGS})
