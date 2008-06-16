add_subdirectory(mplayer)

project(phonon-mplayer)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DPHONON_MPLAYER)

set(phonon_mplayer_SRCS
	${phonon_commom_SRCS}
	MPlayerMediaObject.cpp
	MPlayerMediaController.cpp
	MPlayerVideoWidget.cpp
)

if (KDE4_FOUND)
	kde4_add_plugin(phonon_mplayer ${phonon_mplayer_SRCS})

	target_link_libraries(phonon_mplayer
		${KDE4_KDECORE_LIBS}
		${KDE4_KDEUI_LIBS}
		${QT_PHONON_LIBRARY}
		libmplayer
	)
else (KDE4_FOUND)
	qt4_wrap_cpp(phonon_mplayer_SRCS
		${phonon_commom_MOCS}
		MPlayerMediaObject.h
		MPlayerVideoWidget.h
	)

	add_library(phonon_mplayer MODULE ${phonon_mplayer_SRCS})

	target_link_libraries(phonon_mplayer
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_PHONON_LIBRARY}
		libmplayer
	)
endif (KDE4_FOUND)

if (KDE4_FOUND)
	install(TARGETS phonon_mplayer DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/phonon_backend)
	install(FILES mplayer.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)
	install(FILES icons/16x16/phonon-mplayer.png DESTINATION ${ICON_INSTALL_DIR}/oxygen/16x16/apps)
	install(FILES icons/22x22/phonon-mplayer.png DESTINATION ${ICON_INSTALL_DIR}/oxygen/22x22/apps)
	install(FILES icons/32x32/phonon-mplayer.png DESTINATION ${ICON_INSTALL_DIR}/oxygen/32x32/apps)
	install(FILES icons/48x48/phonon-mplayer.png DESTINATION ${ICON_INSTALL_DIR}/oxygen/48x48/apps)

	#Ubuntu 8.04 Hardy
	install(TARGETS phonon_mplayer DESTINATION /usr/lib/kde4/lib/kde4)
	install(FILES mplayer.desktop DESTINATION /usr/lib/kde4/share/kde4/services/phononbackends)
	install(FILES icons/16x16/phonon-mplayer.png DESTINATION /usr/lib/kde4/share/icons/oxygen/16x16/apps)
	install(FILES icons/22x22/phonon-mplayer.png DESTINATION /usr/lib/kde4/share/icons/oxygen/22x22/apps)
	install(FILES icons/32x32/phonon-mplayer.png DESTINATION /usr/lib/kde4/share/icons/oxygen/32x32/apps)
	install(FILES icons/48x48/phonon-mplayer.png DESTINATION /usr/lib/kde4/share/icons/oxygen/48x48/apps)
else (KDE4_FOUND)
	install(TARGETS phonon_mplayer DESTINATION ${PLUGIN_INSTALL_DIR})
	if (WIN32)
		#install(TARGETS phonon_mplayer DESTINATION ${QT_PLUGINS_DIR}/phonon_backend)
	endif (WIN32)
endif (KDE4_FOUND)
