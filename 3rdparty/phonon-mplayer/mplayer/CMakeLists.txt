add_subdirectory(libmplayer)

project(phonon-mplayer)

if (NEW_TITLE_CHAPTER_HANDLING)
	add_definitions(-DNEW_TITLE_CHAPTER_HANDLING)
endif (NEW_TITLE_CHAPTER_HANDLING)

set(phonon_mplayer_SRCS
	Backend.cpp
	MediaObject.cpp
	MediaController.cpp
	VideoWidget.cpp
	AudioOutput.cpp
	EffectManager.cpp
	Effect.cpp
	SinkNode.cpp
	SeekStack.cpp
)

qt4_wrap_cpp(phonon_mplayer_SRCS
	Backend.h
	MediaObject.h
	MediaController.h
	VideoWidget.h
	AudioOutput.h
	EffectManager.h
	Effect.h
	SinkNode.h
	SeekStack.h
)

add_library(phonon_mplayer MODULE ${phonon_mplayer_SRCS})

# Remove lib prefix under UNIX: libphonon_mplayer.so becomes phonon_mplayer.so
set_target_properties(phonon_mplayer PROPERTIES PREFIX "")

target_link_libraries(phonon_mplayer
	${PHONON_LIBS}
	libmplayer
)

if (WIN32)
	install(TARGETS phonon_mplayer DESTINATION bin/phonon_backend)
else (WIN32)
	install(TARGETS phonon_mplayer DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/phonon_backend)
endif (WIN32)
install(FILES mplayer.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)
