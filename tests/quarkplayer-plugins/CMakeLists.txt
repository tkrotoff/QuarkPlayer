if (STATICPLUGINS)
	add_definitions(-DQT_STATICPLUGIN)
endif (STATICPLUGINS)

add_subdirectory(FindSubtitles)
add_subdirectory(MainWindow)
add_subdirectory(VideoWidget)
add_subdirectory(Playlist)
add_subdirectory(ConfigWindow)
