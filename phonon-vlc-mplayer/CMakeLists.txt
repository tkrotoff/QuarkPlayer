project(phonon-vlc-mplayer)

#phonon-common
set(phonon_commom_SRCS
	../Backend.cpp
	../MediaObject.cpp
	../MediaController.cpp
	../VideoWidget.cpp
	../AudioOutput.cpp
	../EffectManager.cpp
	../Effect.cpp
	../SinkNode.cpp
)

set(phonon_commom_MOCS
	../Backend.h
	../MediaObject.h
	../MediaController.h
	../VideoWidget.h
	../AudioOutput.h
	../EffectManager.h
	../Effect.h
	../SinkNode.h
)
##

set(INSTALL_TARGETS_DEFAULT_ARGS
	RUNTIME DESTINATION ${QT_PLUGINS_DIR}/phonon_backend
	LIBRARY DESTINATION ${QT_PLUGINS_DIR}/phonon_backend
	ARCHIVE DESTINATION ${QT_PLUGINS_DIR}/phonon_backend
)

add_subdirectory(vlc)
add_subdirectory(mplayer)
