project(libmplayer)

set(libmplayer_SRCS
	MyProcess.cpp
	MediaData.cpp
	MediaSettings.cpp
	MPlayerVersion.cpp
	MPlayerProcess.cpp
	MPlayerLoader.cpp
	WidgetNoPaintEvent.cpp
	MPlayerVideoWidget.cpp
	MPlayerConfig.cpp
)

qt4_wrap_cpp(libmplayer_SRCS
	MyProcess.h
	MPlayerProcess.h
	WidgetNoPaintEvent.h
	MPlayerVideoWidget.h
)

add_library(libmplayer STATIC ${libmplayer_SRCS})

# Linking with Qt is required by MinGW, otherwise we get undefined symbols
target_link_libraries(libmplayer
	${PHONON_LIBS}
)
