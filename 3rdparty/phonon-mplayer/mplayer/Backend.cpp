/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2007-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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
#include "PhononMPlayerLogger.h"

#include <QtCore/QByteArray>
#include <QtCore/QSet>
#include <QtCore/QVariant>
#include <QtCore/QtPlugin>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QFutureWatcher>

Q_EXPORT_PLUGIN2(phonon_mplayer, Phonon::MPlayer::Backend);

namespace Phonon
{
namespace MPlayer
{

Backend::Backend(QObject * parent, const QVariantList & args)
	: QObject(parent) {

	Q_UNUSED(args);

	setProperty("identifier", QLatin1String("phonon_mplayer"));
	setProperty("backendName", QLatin1String("MPlayer"));
	setProperty("backendComment", QLatin1String("MPlayer plugin for Phonon"));
	setProperty("backendVersion", QLatin1String("0.1"));
	setProperty("backendWebsite", QLatin1String("http://multimedia.kde.org/"));

	_effectManager = new EffectManager(this);
}

Backend::~Backend() {
}

QObject * Backend::createObject(BackendInterface::Class c, QObject * parent, const QList<QVariant> & args) {
	switch (c) {
	case MediaObjectClass:
		return new MediaObject(parent);

	case AudioOutputClass:
		return new AudioOutput(parent);

	case EffectClass:
		return new Effect(_effectManager, args[0].toInt(), parent);

	case VideoWidgetClass:
		return new VideoWidget(qobject_cast<QWidget *>(parent));

	case VolumeFaderEffectClass:
	case AudioDataOutputClass:
	case VisualizationClass:
	case VideoDataOutputClass:
		PhononMPlayerWarning() << "Not implemented yet:" << c;
		break;
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
	Q_UNUSED(fourcc);

	return true;
}

bool Backend::supportsSubtitles() const {
	return true;
}

QStringList Backend::availableMimeTypes() const {
	if (_supportedMimeTypes.isEmpty()) {
		//Audio mime types
		_supportedMimeTypes
			<< QLatin1String("audio/168sv")
			<< QLatin1String("audio/8svx")
			<< QLatin1String("audio/aiff")
			<< QLatin1String("audio/basic")
			<< QLatin1String("audio/mp3")
			<< QLatin1String("audio/mp4")
			<< QLatin1String("audio/mpeg")
			<< QLatin1String("audio/mpeg2")
			<< QLatin1String("audio/mpeg3")
			<< QLatin1String("audio/vnd.rn-realaudio")
			<< QLatin1String("audio/wav")
			<< QLatin1String("audio/x-16sv")
			<< QLatin1String("audio/x-8svx")
			<< QLatin1String("audio/x-aiff")
			<< QLatin1String("audio/x-basic")
			<< QLatin1String("audio/x-flac")
			<< QLatin1String("audio/x-m4a")
			<< QLatin1String("audio/x-mp3")
			<< QLatin1String("audio/x-mpeg")
			<< QLatin1String("audio/x-mpeg2")
			<< QLatin1String("audio/x-mpeg3")
			<< QLatin1String("audio/x-mpegurl")
			<< QLatin1String("audio/x-ms-wma")
			<< QLatin1String("audio/x-ogg")
			<< QLatin1String("audio/x-pn-aiff")
			<< QLatin1String("audio/x-pn-au")
			<< QLatin1String("audio/x-pn-realaudio-plugin")
			<< QLatin1String("audio/x-pn-wav")
			<< QLatin1String("audio/x-pn-windows-acm")
			<< QLatin1String("audio/x-real-audio")
			<< QLatin1String("audio/x-realaudio")
			<< QLatin1String("audio/x-speex+ogg")
			<< QLatin1String("audio/x-wav");

		//Video mime types
		_supportedMimeTypes
			<< QLatin1String("video/anim")
			<< QLatin1String("video/avi")
			<< QLatin1String("video/mkv")
			<< QLatin1String("video/mng")
			<< QLatin1String("video/mp4")
			<< QLatin1String("video/mpeg")
			<< QLatin1String("video/mpg")
			<< QLatin1String("video/msvideo")
			<< QLatin1String("video/quicktime")
			<< QLatin1String("video/x-anim")
			<< QLatin1String("video/x-flic")
			<< QLatin1String("video/x-mng")
			<< QLatin1String("video/x-mpeg")
			<< QLatin1String("video/x-ms-asf")
			<< QLatin1String("video/x-ms-wmv")
			<< QLatin1String("video/x-msvideo")
			<< QLatin1String("video/x-quicktime");

		//Application mime types
		_supportedMimeTypes
			<< QLatin1String("application/ogg")
			<< QLatin1String("application/vnd.rn-realmedia")
			<< QLatin1String("application/x-annodex")
			<< QLatin1String("application/x-flash-video")
			<< QLatin1String("application/x-quicktimeplayer");

		//Image mime types
		_supportedMimeTypes
			<< QLatin1String("image/ilbm")
			<< QLatin1String("image/png")
			<< QLatin1String("image/x-ilbm")
			<< QLatin1String("image/x-png");
	}

	return _supportedMimeTypes;
}

QList<int> Backend::objectDescriptionIndexes(ObjectDescriptionType type) const {
	PhononMPlayerDebug();

	QList<int> list;

	switch(type) {
	case Phonon::AudioOutputDeviceType:
		list.append(1);
		break;

	case Phonon::EffectType: {
		QList<EffectInfo *> effectList = _effectManager->effectList();
		for (int effect = 0; effect < effectList.size(); ++effect) {
			list.append(effect);
		}
		break;
	}

	case Phonon::AudioCaptureDeviceType:
	case Phonon::AudioChannelType:
#ifdef NEW_TITLE_CHAPTER_HANDLING
	case Phonon::ChapterType:
	case Phonon::TitleType:
#endif	//NEW_TITLE_CHAPTER_HANDLING
	case Phonon::SubtitleType:
		PhononMPlayerWarning() << "Not implemented yet:" << type;
		break;
	}

	return list;
}

QHash<QByteArray, QVariant> Backend::objectDescriptionProperties(ObjectDescriptionType type, int index) const {
	PhononMPlayerDebug();

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

	case Phonon::EffectType: {
		QList<EffectInfo *> effectList = _effectManager->effectList();
		if (index >= 0 && index <= effectList.size()) {
			const EffectInfo * effect = effectList[index];
			ret.insert("name", effect->name());
			ret.insert("command", effect->command());
		} else {
			Q_ASSERT(1);	//Since we use list position as ID, this should not happen
		}

		break;
	}

	case Phonon::AudioCaptureDeviceType:
		PhononMPlayerWarning() << "Not implemented yet:" << type;
		break;

	default:
		PhononMPlayerCritical() << "Unknow ObjectDescriptionType:" << type;
	}

	return ret;
}

bool Backend::startConnectionChange(QSet<QObject *> nodes) {
	PhononMPlayerDebug();
	foreach (QObject * node, nodes) {
		PhononMPlayerDebug() << "Node:" << node->metaObject()->className();
	}

	Q_UNUSED(nodes);
	//There's nothing we can do but hope the connection changes won't take too long so that buffers
	//would underrun. But we should be pretty safe the way xine works by not doing anything here.
	return true;
}

bool Backend::connectNodes(QObject * source, QObject * sink) {
	PhononMPlayerDebug() << source->metaObject()->className() << sink->metaObject()->className();

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
			//Effect * effect = qobject_cast<Effect *>(source);
			//Nothing todo, MPlayer does not support this kind of connection
			return true;
		}
	}

	PhononMPlayerWarning() << "Connection not supported";
	return false;
}

bool Backend::disconnectNodes(QObject * source, QObject * sink) {
	PhononMPlayerDebug() << "Source:" << source->metaObject()->className() << "sink:" << sink->metaObject()->className();

	SinkNode * sinkNode = qobject_cast<SinkNode *>(sink);
	if (sinkNode) {
		MediaObject * mediaObject = qobject_cast<MediaObject *>(source);
		if (mediaObject) {
			//Disconnects the SinkNode from a MediaObject
			sinkNode->disconnectFromMediaObject(mediaObject);
			return true;
		} else {
			//FIXME try to find a better way...
			//Effect * effect = qobject_cast<Effect * >(source);
			//Nothing todo, MPlayer does not support this kind of connection
			return true;
		}

	}

	PhononMPlayerWarning() << "Disconnection not supported";
	return false;
}

bool Backend::endConnectionChange(QSet<QObject *> nodes) {
	PhononMPlayerDebug();
	foreach (QObject * node, nodes) {
		PhononMPlayerDebug() << "Node:" << node->metaObject()->className();
	}

	return true;
}

void Backend::freeSoundcardDevices() {
}

}}	//Namespace Phonon::MPlayer
