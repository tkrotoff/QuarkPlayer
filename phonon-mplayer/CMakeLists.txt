add_subdirectory(libmplayer)

project(phonon-mplayer)

# Copy config.h to the current source directory
# since build dir in not inside the include path
# If it needed, this can be changed
configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

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
	WidgetNoPaintEvent.cpp
	MPlayerVideoWidget.cpp
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
	WidgetNoPaintEvent.h
	MPlayerVideoWidget.h
)

add_library(phonon_mplayer MODULE ${phonon_mplayer_SRCS})

target_link_libraries(phonon_mplayer
	${PHONON_LIBS}
	libmplayer
)

install(TARGETS phonon_mplayer DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/phonon_backend)
install(FILES mplayer.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)
