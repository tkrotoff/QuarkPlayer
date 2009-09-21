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
	Config.cpp
)

qt4_wrap_cpp(libmplayer_SRCS
	MyProcess.h
	MPlayerProcess.h
	WidgetNoPaintEvent.h
	MPlayerVideoWidget.h
)

add_library(libmplayer STATIC ${libmplayer_SRCS})
