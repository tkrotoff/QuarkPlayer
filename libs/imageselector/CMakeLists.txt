project(imageselector)

add_definitions(-DBUILD_IMAGESELECTOR)

set(imageselector_SRCS
	ImageSelector.cpp
	ThumbnailDirModel.cpp
	ThumbnailManager.cpp
	ThumbnailView.cpp
)

qt4_wrap_cpp(imageselector_SRCS
	ImageSelector.h
	ThumbnailDirModel.h
	ThumbnailManager.h
	ThumbnailView.h
)

qt4_wrap_ui(imageselector_SRCS
	ImageSelector.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(imageselector STATIC ${imageselector_SRCS})

target_link_libraries(imageselector
	${QT_QTCORE_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

install(TARGETS imageselector DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
