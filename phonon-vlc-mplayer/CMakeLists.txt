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
	../VideoWidget.h
	../AudioOutput.h
	../EffectManager.h
	../Effect.h
	../SinkNode.h
)
##

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README DESTINATION ".")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/COPYING DESTINATION ".")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/COPYING.LESSER DESTINATION ".")

add_subdirectory(vlc)
add_subdirectory(mplayer)
