project(phonon-vlc-mplayer)

#phonon-common
set(phonon_commom_SRCS
	../Backend.cpp
	../MediaObject.cpp
	../VideoWidget.cpp
	../AudioOutput.cpp
	../EffectManager.cpp
	../Effect.cpp
	../SinkNode.cpp
)

set(phonon_commom_MOCS
	../Backend.h
	../MediaObject.h
	../VideoWidget.h
	../AudioOutput.h
	../EffectManager.h
	../Effect.h
	../SinkNode.h
)
##

add_subdirectory(vlc)
add_subdirectory(mplayer)
