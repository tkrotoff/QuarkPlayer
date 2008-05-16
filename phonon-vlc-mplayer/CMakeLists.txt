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
	../SeekStack.cpp
)

set(phonon_commom_MOCS
	../Backend.h
	../MediaObject.h
	../VideoWidget.h
	../AudioOutput.h
	../EffectManager.h
	../Effect.h
	../SinkNode.h
	../SeekStack.h
)
##

if (WIN32)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README DESTINATION ".")
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/COPYING DESTINATION ".")
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/COPYING.LESSER DESTINATION ".")
endif (WIN32)

add_subdirectory(vlc)
add_subdirectory(mplayer)
