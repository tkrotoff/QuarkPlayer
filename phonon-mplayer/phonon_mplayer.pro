TEMPLATE = lib
DESTDIR = phonon_backend
QT += phonon
CONFIG += qt plugin
TARGET = phonon_mplayer

DEFINES += PHONON_MAKE_QT_ONLY_BACKEND
PHONON_MPLAYER_DIR = "."
LIBMPLAYER_DIR = "./libmplayer"

# Input
HEADERS += \
	$$LIBMPLAYER_DIR/MyProcess.h \
	$$LIBMPLAYER_DIR/MPlayerProcess.h \
	$$PHONON_MPLAYER_DIR/Backend.h \
	$$PHONON_MPLAYER_DIR/MediaObject.h \
	$$PHONON_MPLAYER_DIR/MediaController.h \
	$$PHONON_MPLAYER_DIR/VideoWidget.h \
	$$PHONON_MPLAYER_DIR/AudioOutput.h \
	$$PHONON_MPLAYER_DIR/EffectManager.h \
	$$PHONON_MPLAYER_DIR/Effect.h \
	$$PHONON_MPLAYER_DIR/SinkNode.h \
	$$PHONON_MPLAYER_DIR/SeekStack.h \
	$$PHONON_MPLAYER_DIR/WidgetNoPaintEvent.h \
	$$PHONON_MPLAYER_DIR/MPlayerVideoWidget.h

SOURCES += \
	$$LIBMPLAYER_DIR/MyProcess.cpp \
	$$LIBMPLAYER_DIR/MediaData.cpp \
	$$LIBMPLAYER_DIR/MediaSettings.cpp \
	$$LIBMPLAYER_DIR/MPlayerVersion.cpp \
	$$LIBMPLAYER_DIR/MPlayerProcess.cpp \
	$$LIBMPLAYER_DIR/MPlayerLoader.cpp \
	$$PHONON_MPLAYER_DIR/Backend.cpp \
	$$PHONON_MPLAYER_DIR/MediaObject.cpp \
	$$PHONON_MPLAYER_DIR/MediaController.cpp \
	$$PHONON_MPLAYER_DIR/VideoWidget.cpp \
	$$PHONON_MPLAYER_DIR/AudioOutput.cpp \
	$$PHONON_MPLAYER_DIR/EffectManager.cpp \
	$$PHONON_MPLAYER_DIR/Effect.cpp \
	$$PHONON_MPLAYER_DIR/SinkNode.cpp \
	$$PHONON_MPLAYER_DIR/SeekStack.cpp \
	$$PHONON_MPLAYER_DIR/WidgetNoPaintEvent.cpp \
	$$PHONON_MPLAYER_DIR/MPlayerVideoWidget.cpp
