TEMPLATE = lib
DESTDIR = phonon_backend
QT += phonon
CONFIG += qt plugin
TARGET = phonon_mplayer

DEFINES += PHONON_MAKE_QT_ONLY_BACKEND

HEADERS += \
	libmplayer/MyProcess.h \
	libmplayer/MPlayerProcess.h \
	libmplayer/WidgetNoPaintEvent.h \
	libmplayer/MPlayerVideoWidget.h \
	libmplayer/MPlayerConfig.h \
	Backend.h \
	MediaObject.h \
	MediaController.h \
	VideoWidget.h \
	AudioOutput.h \
	EffectManager.h \
	Effect.h \
	SinkNode.h \
	SeekStack.h

SOURCES += \
	libmplayer/MyProcess.cpp \
	libmplayer/MediaData.cpp \
	libmplayer/MediaSettings.cpp \
	libmplayer/MPlayerVersion.cpp \
	libmplayer/MPlayerProcess.cpp \
	libmplayer/MPlayerLoader.cpp \
	libmplayer/WidgetNoPaintEvent.cpp \
	libmplayer/MPlayerVideoWidget.cpp \
	libmplayer/MPlayerConfig.cpp \
	Backend.cpp \
	MediaObject.cpp \
	MediaController.cpp \
	VideoWidget.cpp \
	AudioOutput.cpp \
	EffectManager.cpp \
	Effect.cpp \
	SinkNode.cpp \
	SeekStack.cpp
