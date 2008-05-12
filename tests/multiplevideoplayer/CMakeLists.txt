project(multiplevideoplayer)

#add_definitions(-DNEW_TITLE_CHAPTER_HANDLING)

set(multiplevideoplayer_SRCS
	main.cpp
	MainWindow.cpp
	VideoWindow.cpp
)

qt4_add_resources(multiplevideoplayer_SRCS
	multiplevideoplayer.qrc
)

qt4_wrap_cpp(multiplevideoplayer_SRCS
	MainWindow.h
	VideoWindow.h
)

qt4_wrap_ui(multiplevideoplayer_SRCS
	MainWindow.ui
	VideoWindow.ui
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(multiplevideoplayer ${multiplevideoplayer_SRCS})

if (KDE4_FOUND)
	target_link_libraries(multiplevideoplayer
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_PHONON_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(multiplevideoplayer
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_PHONON_LIBRARY}
	)
endif (KDE4_FOUND)
