project(phonon-vlc)

find_package(KDE4 REQUIRED)
if (KDE4_FOUND)
	include(KDE4Defaults)
	include(MacroLibrary)
else (KDE4_FOUND)
	find_package(Qt4 REQUIRED)
endif (KDE4_FOUND)

message(STATUS "KDE4 Support: ${KDE4_FOUND}")
message(STATUS "KDE4 Dir: ${KDE4_DATA_DIR}")

include_directories(${QT_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (KDE4_FOUND)
	add_definitions(${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS} -DKDE4_FOUND)
	include_directories(${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
endif (KDE4_FOUND)

set(phonon_vlc_SRCS
	Backend.cpp
	MediaObject.cpp
	VideoWidget.cpp
	AudioOutput.cpp
	vlc_loader.cpp
	VLCMediaObject.cpp
	vlc_symbols.cpp
)

if (NOT KDE4_FOUND)
	qt4_wrap_cpp(phonon_vlc_SRCS
		Backend.h
		MediaObject.h
		VideoWidget.h
		AudioOutput.h
		VLCMediaObject.h
	)
endif (NOT KDE4_FOUND)

if (KDE4_FOUND)
	kde4_add_plugin(phonon_vlc ${phonon_vlc_SRCS})
else (KDE4_FOUND)
	add_library(phonon_vlc MODULE ${phonon_vlc_SRCS})
endif (KDE4_FOUND)

if (UNIX)
	if (KDE4_FOUND)
		set(QT_PHONON_LIBRARY_DEBUG
			${KDE4_PHONON_LIBS}
		)
	else (KDE4_FOUND)
		set(QT_PHONON_LIBRARY_DEBUG
			${QT_LIBRARY_DIR}/libphonon.so.4.1.0.debug
		)
	endif (KDE4_FOUND)
endif (UNIX)

if (WIN32)
	set(QT_PHONON_LIBRARY_DEBUG
		${QT_LIBRARY_DIR}/phonond4.lib
	)
endif (WIN32)

if (KDE4_FOUND)
	target_link_libraries(phonon_vlc
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${KDE4_PHONON_LIBS}
	)
else (KDE4_FOUND)
	target_link_libraries(phonon_vlc
		${QT_QTCORE_LIBRARY_DEBUG}
		${QT_QTGUI_LIBRARY_DEBUG}
		${QT_PHONON_LIBRARY_DEBUG}
	)
endif (KDE4_FOUND)

if (KDE4_FOUND)
	install(TARGETS phonon_vlc DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/phonon_backend)
	install(FILES vlc.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)

	#Ubuntu 8.04 Hardy
	install(TARGETS phonon_vlc DESTINATION /usr/lib/kde4/lib/kde4)
	install(FILES vlc.desktop DESTINATION /usr/lib/kde4/share/kde4/services/phononbackends)
else (KDE4_FOUND)
	install(TARGETS phonon_vlc DESTINATION ${QT_PLUGINS_DIR}/phonon_backend)
endif (KDE4_FOUND)
