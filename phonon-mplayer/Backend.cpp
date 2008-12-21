/*
 * MPlayer backend for the Phonon library
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
#include "EffectManager.h"
#include "Effect.h"
#include "config.h"

#include <QtCore/QByteArray>
#include <QtCore/QSet>
#include <QtCore/QVariant>
#include <QtCore/QtPlugin>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QFutureWatcher>

#ifdef KDE4_FOUND
	#include <kpluginfactory.h>
	#include <kpluginloader.h>
#endif	//KDE4_FOUND

#ifdef KDE4_FOUND
	K_PLUGIN_FACTORY(MPlayerBackendFactory, registerPlugin<Phonon::MPlayer::Backend>();)
	K_EXPORT_PLUGIN(MPlayerBackendFactory("mplayerbackend"))
#else
	Q_EXPORT_PLUGIN2(phonon_mplayer, Phonon::MPlayer::Backend);
#endif	//KDE4_FOUND

namespace Phonon
{
namespace MPlayer
{

Backend::Backend(QObject * parent, const QVariantList & args)
	: QObject(parent) {

	setProperty("identifier", QLatin1String("phonon_mplayer"));
	setProperty("backendName", QLatin1String("MPlayer"));
	setProperty("backendComment", QLatin1String("MPlayer plugin for Phonon"));
	setProperty("backendVersion", QLatin1String("0.1"));
	setProperty("backendWebsite", QLatin1String("http://multimedia.kde.org/"));

	_effectManager = new EffectManager(this);
}

Backend::~Backend() {
	delete _effectManager;
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
	case EffectClass: {
		return new Effect(_effectManager, args[0].toInt(), parent);
	}
	case VideoWidgetClass:
		return new VideoWidget(qobject_cast<QWidget *>(parent));
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

	switch(type) {
	case Phonon::AudioOutputDeviceType:
		list.append(1);
		break;
	/*case Phonon::AudioCaptureDeviceType:
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
		break;*/

	case Phonon::EffectType:
		QList<EffectInfo *> effectList = _effectManager->effectList();
		for (int effect = 0; effect < effectList.size(); ++effect) {
			list.append(effect);
		}
		break;

	}

	return list;
}

QHash<QByteArray, QVariant> Backend::objectDescriptionProperties(ObjectDescriptionType type, int index) const {
	qDebug() << __FUNCTION__;

	QHash<QByteArray, QVariant> ret;

	switch (type) {
	case Phonon::AudioOutputDeviceType:
		//For MPlayer:
		//mplayer *.mp3
		//mplayer -ao oss *.mp3
		//mplayer -ao alsa *.mp3
		//mplayer -ao oss:/dev/dsp *.mp3
		//mplayer -ao alsa:device=hw=0.0 *.mp3
		//mplayer -ao oss:/dev/dsp1 *.mp3
		//mplayer -ao alsa:device=hw=1.0 *.mp3
		//See http://linux.dsplabs.com.au/mplayer-multiple-sound-cards-select-audio-device-p77/
		//mplayer -ao dsound:device=0
		ret.insert("device", "0");
		break;
	/*case Phonon::AudioCaptureDeviceType:
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
	*/

	case Phonon::EffectType: {
		QList<EffectInfo *> effectList = _effectManager->effectList();
		if (index >= 0 && index <= effectList.size()) {
			const EffectInfo * effect = effectList[index];
			ret.insert("name", effect->name());
			ret.insert("command", effect->command());
		} else
			Q_ASSERT(1); // Since we use list position as ID, this should not happen

		break;
	}

	default:
		qCritical() << __FUNCTION__ << "Unknow ObjectDescriptionType:" << type;
	}

	return ret;
}

bool Backend::startConnectionChange(QSet<QObject *> nodes) {
	qDebug() << __FUNCTION__;
	foreach (QObject * node, nodes) {
		qDebug() << __FUNCTION__ << "Node:" << node->metaObject()->className();
	}

	Q_UNUSED(nodes);
	//There's nothing we can do but hope the connection changes won't take too long so that buffers
	//would underrun. But we should be pretty safe the way xine works by not doing anything here.
	return true;
}

bool Backend::connectNodes(QObject * source, QObject * sink) {
	qDebug() << __FUNCTION__ << source->metaObject()->className() << sink->metaObject()->className();

	//Example:
	//source = Phonon::MPlayer::MediaObject
	//sink = Phonon::MPlayer::VideoWidget

	//Example:
	//source = Phonon::MPlayer::MediaObject
	//sink = Phonon::MPlayer::AudioOutput

	//Example:
	//source = Phonon::MPlayer::MediaObject
	//sink = Phonon::MPlayer::Effect

	//Example:
	//source = Phonon::MPlayer::Effect
	//sink = Phonon::MPlayer::AudioOutput

	SinkNode * sinkNode = qobject_cast<SinkNode *>(sink);
	if (sinkNode) {
		MediaObject * mediaObject = qobject_cast<MediaObject *>(source);
		if (mediaObject) {
			//Connects the SinkNode to a MediaObject
			sinkNode->connectToMediaObject(mediaObject);
			return true;
		} else {
			//FIXME try to find a better way...
			Effect * effect = qobject_cast<Effect * >(source);
			//Nothing todo, MPlayer does not support this kind of connection
			return true;
		}
	}

	qWarning() << __FUNCTION__ << "Connection not supported";
	return false;
}

bool Backend::disconnectNodes(QObject * source, QObject * sink) {
	qDebug() << __FUNCTION__ << "Source:" << source->metaObject()->className() << "sink:" << sink->metaObject()->className();

	SinkNode * sinkNode = qobject_cast<SinkNode *>(sink);
	if (sinkNode) {
		MediaObject * mediaObject = qobject_cast<MediaObject *>(source);
		if (mediaObject) {
			//Disconnects the SinkNode from a MediaObject
			sinkNode->disconnectFromMediaObject(mediaObject);
			return true;
		} else {
			//FIXME try to find a better way...
			Effect * effect = qobject_cast<Effect * >(source);
			//Nothing todo, MPlayer does not support this kind of connection
			return true;
		}

	}

	qWarning() << __FUNCTION__ << "Error: disconnection not supported";
	return false;
}

bool Backend::endConnectionChange(QSet<QObject *> nodes) {
	qDebug() << __FUNCTION__;
	foreach (QObject * node, nodes) {
		qDebug() << __FUNCTION__ << "Node:" << node->metaObject()->className();
	}

	return true;
}

void Backend::freeSoundcardDevices() {
}

}}	//Namespace Phonon::MPlayer
