/*
 * VLC and MPlayer backends for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Backend.h"

#include "MediaObject.h"
#include "VideoWidget.h"
#include "AudioOutput.h"

#ifdef PHONON_VLC
	#include "vlc_loader.h"
	#include "vlc_symbols.h"
#endif	//PHONON_VLC

#include <QtCore/QByteArray>
#include <QtCore/QSet>
#include <QtCore/QVariant>
#include <QtCore/QtPlugin>

#ifndef KDE4_FOUND
	#include <QtCore/QtConcurrentRun>
	#include <QtCore/QFutureWatcher>
#endif	//!KDE4_FOUND

#ifdef KDE4_FOUND
	#include <kpluginfactory.h>
	#include <kpluginloader.h>
#endif	//KDE4_FOUND

#ifdef PHONON_VLC
#ifdef KDE4_FOUND
	K_PLUGIN_FACTORY(VLCBackendFactory, registerPlugin<Phonon::VLC_MPlayer::Backend>();)
	K_EXPORT_PLUGIN(VLCBackendFactory("vlcbackend"))
#else
	Q_EXPORT_PLUGIN2(phonon_vlc, Phonon::VLC_MPlayer::Backend);
#endif	//KDE4_FOUND
#endif	//PHONON_VLC

#ifdef PHONON_MPLAYER
#ifdef KDE4_FOUND
	K_PLUGIN_FACTORY(MPlayerBackendFactory, registerPlugin<Phonon::VLC_MPlayer::Backend>();)
	K_EXPORT_PLUGIN(MPlayerBackendFactory("mplayerbackend"))
#else
	Q_EXPORT_PLUGIN2(phonon_mplayer, Phonon::VLC_MPlayer::Backend);
#endif	//KDE4_FOUND
#endif	//PHONON_MPLAYER

namespace Phonon
{
namespace VLC_MPlayer
{

Backend::Backend(QObject * parent, const QVariantList &)
	: QObject(parent) {

#ifdef PHONON_VLC
	setProperty("identifier", QLatin1String("phonon_vlc"));
	setProperty("backendName", QLatin1String("VLC"));
	setProperty("backendComment", QLatin1String("VLC plugin for Phonon"));
	setProperty("backendVersion", QLatin1String("0.1"));
	setProperty("backendWebsite", QLatin1String("http://multimedia.kde.org/"));

	qDebug() << "Loading VLC...";

#ifndef KDE4_FOUND
	//Before everything else
	//QtConcurrent runs initLibVLC() in another thread
	//Otherwise it takes to long loading all VLC plugins
	QFutureWatcher<void> watcher;
	connect(&watcher, SIGNAL(finished()),
		SLOT(initLibVLCFinished()));
	QFuture<void> _initLibVLCFuture = QtConcurrent::run(initLibVLC);
	watcher.setFuture(_initLibVLCFuture);
#else
	initLibVLC();
#endif	//!KDE4_FOUND

#endif	//PHONON_VLC

#ifdef PHONON_MPLAYER
	setProperty("identifier", QLatin1String("phonon_mplayer"));
	setProperty("backendName", QLatin1String("MPlayer"));
	setProperty("backendComment", QLatin1String("MPlayer plugin for Phonon"));
	setProperty("backendVersion", QLatin1String("0.1"));
	setProperty("backendWebsite", QLatin1String("http://multimedia.kde.org/"));

	qDebug() << "Using MPlayer version:" << "not yet implemented";
#endif	//PHONON_MPLAYER
}

Backend::~Backend() {
	//releaseLibVLC();
}

void Backend::initLibVLCFinished() {
#ifdef PHONON_VLC
	qDebug() << "Using VLC version:" << p_libvlc_get_version();
	qDebug() << "VLC loaded";
#endif	//PHONON_VLC
}

QObject * Backend::createObject(BackendInterface::Class c, QObject * parent, const QList<QVariant> & args) {
	switch (c) {
	case MediaObjectClass:
		return new MediaObject(parent);
	/*case VolumeFaderEffectClass:
		return new VolumeFaderEffect(parent);
	*/
	case AudioOutputClass:
		return new AudioOutput(parent);
	/*case AudioDataOutputClass:
		return new AudioDataOutput(parent);
	case VisualizationClass:
		return new Visualization(parent);
	case VideoDataOutputClass:
		return new VideoDataOutput(parent);*/
	/*case EffectClass: {
		Q_ASSERT(args.size() == 1);
		qDebug() << "creating Effect(" << args[0];
		Effect * effect = new Effect(args[0].toInt(), parent);
		if (effect->isValid()) {
			return effect;
		}
		delete effect;
		return NULL;
	}*/
	case VideoWidgetClass: {
		VideoWidget * videoWidget = new VideoWidget(qobject_cast<QWidget *>(parent));
		return videoWidget;
	}
	}
	return NULL;
}

bool Backend::supportsVideo() const {
	return true;
}

bool Backend::supportsOSD() const {
	return true;
}

bool Backend::supportsFourcc(quint32 fourcc) const {
	return true;
}

bool Backend::supportsSubtitles() const {
	return true;
}

QStringList Backend::availableMimeTypes() const {
	if (_supportedMimeTypes.isEmpty()) {
		//Audio mime types
		_supportedMimeTypes
			<< "audio/x-m4a"
			<< "audio/x-aiff"
			<< "audio/aiff"
			<< "audio/x-pn-aiff"
			<< "audio/x-realaudio"
			<< "audio/basic"
			<< "audio/x-basic"
			<< "audio/x-pn-au"
			<< "audio/x-8svx"
			<< "audio/8svx"
			<< "audio/x-16sv"
			<< "audio/168sv"
			<< "audio/x-ogg"
			<< "audio/x-speex+ogg"
			<< "audio/vnd.rn-realaudio"
			<< "audio/x-pn-realaudio-plugin"
			<< "audio/x-real-audio"
			<< "audio/x-wav"
			<< "audio/wav"
			<< "audio/x-pn-wav"
			<< "audio/x-pn-windows-acm"
			<< "audio/mpeg2"
			<< "audio/x-mpeg2"
			<< "audio/mpeg3"
			<< "audio/x-mpeg3"
			<< "audio/mpeg"
			<< "audio/x-mpeg"
			<< "audio/x-mpegurl"
			<< "audio/x-mp3"
			<< "audio/mp3"
			<< "audio/mpeg"
			<< "audio/x-ms-wma";

		//Video mime types
		_supportedMimeTypes
			<< "video/quicktime"
			<< "video/x-quicktime"
			<< "video/mkv"
			<< "video/msvideo"
			<< "video/x-msvideo"
			<< "video/x-flic"
			<< "video/x-anim"
			<< "video/anim"
			<< "video/mng"
			<< "video/x-mng"
			<< "video/mpeg"
			<< "video/x-mpeg"
			<< "video/x-ms-asf"
			<< "video/x-ms-wmv"
			<< "video/mp4"
			<< "video/mpg"
			<< "video/avi";

		//Application mime types
		_supportedMimeTypes
			<< "application/x-annodex"
			<< "application/x-quicktimeplayer"
			<< "application/ogg"
			<< "application/ogg"
			<< "application/vnd.rn-realmedia"
			<< "application/x-flash-video";

		//Image mime types
		_supportedMimeTypes
			<< "image/x-ilbm"
			<< "image/ilbm"
			<< "image/png"
			<< "image/x-png";
	}

	return _supportedMimeTypes;
}

QList<int> Backend::objectDescriptionIndexes(ObjectDescriptionType type) const {
	qDebug() << __FUNCTION__ << "";

	QList<int> list;

	/*switch(type) {
	case Phonon::AudioOutputDeviceType:
		break;
	case Phonon::AudioCaptureDeviceType:
		break;
	case Phonon::VideoOutputDeviceType:
		break;
	case Phonon::VideoCaptureDeviceType:
		break;
	case Phonon::VisualizationType:
		break;
	case Phonon::AudioCodecType:
		break;
	case Phonon::VideoCodecType:
		break;
	case Phonon::ContainerFormatType:
		break;
	case Phonon::EffectType:
		break;
	}*/

	return list;
}

QHash<QByteArray, QVariant> Backend::objectDescriptionProperties(ObjectDescriptionType type, int index) const {
	qDebug() << __FUNCTION__ << "";

	QHash<QByteArray, QVariant> ret;

	/*switch (type) {
	case Phonon::AudioOutputDeviceType:
		break;
	case Phonon::AudioCaptureDeviceType:
		break;
	case Phonon::VideoOutputDeviceType:
		break;
	case Phonon::VideoCaptureDeviceType:
		break;
	case Phonon::VisualizationType:
		break;
	case Phonon::AudioCodecType:
		break;
	case Phonon::VideoCodecType:
		break;
	case Phonon::ContainerFormatType:
		break;
	case Phonon::EffectType:
		break;
	}*/

	return ret;
}

bool Backend::startConnectionChange(QSet<QObject *> nodes) {
	qDebug() << __FUNCTION__ << "";
	foreach (QObject * node, nodes) {
		qDebug() << "node:" << node->metaObject()->className();
	}

	Q_UNUSED(nodes);
	//There's nothing we can do but hope the connection changes won't take too long so that buffers
	//would underrun. But we should be pretty safe the way xine works by not doing anything here.
	return true;
}

bool Backend::connectNodes(QObject * source, QObject * sink) {
	qDebug() << __FUNCTION__ << source->metaObject()->className() << sink->metaObject()->className();

	//Example:
	//source = Phonon::VLC_MPlayer::MediaObject (inherits Phonon::MediaNode)
	//sink = Phonon::VLC_MPlayer::VideoWidget (inherits Phonon::MediaNode)
	VideoWidget * videoWidget = qobject_cast<VideoWidget *>(sink);
	if (videoWidget) {
		//We have a VideoWidget
		MediaObject * mediaObject = qobject_cast<MediaObject *>(source);
		if (mediaObject) {
			//Connects the VideoWidget to a MediaObject
			videoWidget->connectToMediaObject(mediaObject);
			return true;
		}
	}

	//Example:
	//source = Phonon::VLC_MPlayer::MediaObject (inherits Phonon::MediaNode)
	//sink = Phonon::VLC_MPlayer::AudioOutput (inherits Phonon::MediaNode)
	AudioOutput * audioOutput = qobject_cast<AudioOutput *>(sink);
	if (audioOutput) {
		//We have a AudioOutput
		MediaObject * mediaObject = qobject_cast<MediaObject *>(source);
		if (mediaObject) {
			//Connects the AudioOutput to a MediaObject
			audioOutput->connectToMediaObject(mediaObject);
			return true;
		}
	}

	qWarning() << __FUNCTION__ << "Connection not supported";
	return false;
}

bool Backend::disconnectNodes(QObject * _source, QObject * _sink) {
	qDebug() << __FUNCTION__ << _source->metaObject()->className() << "" << _sink->metaObject()->className();

	return true;
}

bool Backend::endConnectionChange(QSet<QObject *> nodes) {
	qDebug() << __FUNCTION__ << "";
	foreach (QObject * node, nodes) {
		qDebug() << "node:" << node->metaObject()->className();
	}

	return true;
}

void Backend::freeSoundcardDevices() {
}

QString Backend::toString() const {
#ifdef PHONON_VLC
	return "VLC Phonon Backend by Tanguy Krotoff <tkrotoff@gmail.com>\n";
		/*"libvlc version=" + QString(p_libvlc_get_version()) + "\n"
		"libvlc changeset=" + QString(p_libvlc_get_changeset()) + "\n"
		"libvlc compiler=" + QString(p_libvlc_get_compiler());*/
#endif	//PHONON_VLC

#ifdef PHONON_MPLAYER
	return "MPlayer Phonon Backend by Tanguy Krotoff <tkrotoff@gmail.com>";
#endif	//PHONON_MPLAYER
}

}}	//Namespace Phonon::VLC_MPlayer
