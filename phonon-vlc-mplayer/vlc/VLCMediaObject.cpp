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

#include "VLCMediaObject.h"

#include "../VideoWidget.h"

#include "vlc_loader.h"
#include "vlc_symbols.h"

#include <QtCore/QTimer>
#include <QtCore/QtDebug>

namespace Phonon
{
namespace VLC_MPlayer
{

//VLC returns a strange position... have to multiply by VLC_POSITION_RESOLUTION
static const int VLC_POSITION_RESOLUTION = 1000;

VLCMediaObject::VLCMediaObject(QObject * parent)
	: MediaObject(parent), VLCMediaController() {

	//MediaPlayer
	_vlcMediaPlayer = NULL;
	_vlcMediaPlayerEventManager = NULL;

	//Media
	_vlcMedia = NULL;
	_vlcMediaEventManager = NULL;

	//MediaDiscoverer
	_vlcMediaDiscoverer = NULL;
	_vlcMediaDiscovererEventManager = NULL;

	_totalTime = 0;
	_hasVideo = false;
	_seekable = false;
}

VLCMediaObject::~VLCMediaObject() {
	//unloadMedia();
}

void VLCMediaObject::unloadMedia() {
	if (_vlcMediaPlayer) {
		p_libvlc_media_player_release(_vlcMediaPlayer);
		_vlcMediaPlayer = NULL;
	}

	if (_vlcMedia) {
		p_libvlc_media_release(_vlcMedia);
		_vlcMedia = NULL;
	}
}

void VLCMediaObject::loadMediaInternal(const QString & filename) {
	_initLibVLCFuture.waitForFinished();

	qDebug() << (int) this << "loadMedia()" << filename;

	//Create a new media from a filename
	_vlcMedia = p_libvlc_media_new(_vlcInstance, filename.toAscii(), _vlcException);
	checkException();

	//Create a media player environement
	_vlcMediaPlayer = p_libvlc_media_player_new_from_media(_vlcMedia, _vlcException);
	checkException();

	//No need to keep the media now
	//p_libvlc_media_release(_vlcMedia);

	//connectToAllVLCEvents() at the end since it needs _vlcMediaPlayer
	connectToAllVLCEvents();

	//Gets meta data (artist, title...)
	updateMetaData();
}

void VLCMediaObject::setVLCWidgetId() {
	//Get our media player to use our window
	//FIXME This code does not work inside libvlc!
	//p_libvlc_media_player_set_drawable(_vlcMediaPlayer, (libvlc_drawable_t) _videoWidgetId, _vlcException);
	//checkException();

	p_libvlc_video_set_parent(_vlcInstance, (libvlc_drawable_t) _videoWidgetId, _vlcException);
	checkException();
}

void VLCMediaObject::playInternal() {
	_vlcCurrentMediaPlayer = _vlcMediaPlayer;

	setVLCWidgetId();

	//Play
	p_libvlc_media_player_play(_vlcMediaPlayer, _vlcException);
	checkException();
}

void VLCMediaObject::pause() {
	p_libvlc_media_player_pause(_vlcMediaPlayer, _vlcException);
	checkException();
}

void VLCMediaObject::stopInternal() {
	//FIXME
	//inside libvlc, does not check if in playing or paused state
	p_libvlc_media_player_stop(_vlcMediaPlayer, _vlcException);
	checkException();
	//unloadMedia();
}

void VLCMediaObject::seek(qint64 milliseconds) {
	qDebug() << (int) this << "seek() milliseconds:" << milliseconds;
	p_libvlc_media_player_set_time(_vlcMediaPlayer, milliseconds, _vlcException);
	checkException();
}

QString VLCMediaObject::errorString() const {
	return p_libvlc_exception_get_message(_vlcException);
}

bool VLCMediaObject::hasVideo() const {
	bool hasVideo = p_libvlc_media_player_has_vout(_vlcMediaPlayer, _vlcException);
	checkException();

	return hasVideo;
}

bool VLCMediaObject::isSeekable() const {
	bool isSeekable = p_libvlc_media_player_is_seekable(_vlcMediaPlayer, _vlcException);
	checkException();

	return isSeekable;
}

void VLCMediaObject::connectToAllVLCEvents() {

	//MediaPlayer
	_vlcMediaPlayerEventManager = p_libvlc_media_player_event_manager(_vlcMediaPlayer, _vlcException);
	libvlc_event_type_t eventsMediaPlayer[] = {
		libvlc_MediaPlayerPlayed,
		libvlc_MediaPlayerPaused,
		libvlc_MediaPlayerEndReached,
		libvlc_MediaPlayerStopped,
		libvlc_MediaPlayerEncounteredError,
		libvlc_MediaPlayerTimeChanged,
		libvlc_MediaPlayerPositionChanged,
		libvlc_MediaPlayerSeekableChanged,
		libvlc_MediaPlayerPausableChanged,
	};
	int nbEvents = sizeof(eventsMediaPlayer) / sizeof(*eventsMediaPlayer);
	for (int i = 0; i < nbEvents; i++) {
		p_libvlc_event_attach(_vlcMediaPlayerEventManager, eventsMediaPlayer[i], libvlc_callback, this, _vlcException);
	}


	//Media
	_vlcMediaEventManager = p_libvlc_media_event_manager(_vlcMedia, _vlcException);
	libvlc_event_type_t eventsMedia[] = {
		libvlc_MediaMetaChanged,
		libvlc_MediaSubItemAdded,
		libvlc_MediaDurationChanged,
		//FIXME libvlc does not know this event
		//libvlc_MediaPreparsedChanged,
		libvlc_MediaFreed,
		libvlc_MediaStateChanged,
	};
	nbEvents = sizeof(eventsMedia) / sizeof(*eventsMedia);
	for (int i = 0; i < nbEvents; i++) {
		p_libvlc_event_attach(_vlcMediaEventManager, eventsMedia[i], libvlc_callback, this, _vlcException);
		checkException();
	}


	//MediaDiscoverer
	//FIXME why libvlc_media_discoverer_event_manager() does not take a libvlc_exception_t?
	/*
	_vlcMediaDiscovererEventManager = p_libvlc_media_discoverer_event_manager(_vlcMediaDiscoverer);
	libvlc_event_type_t eventsMediaDiscoverer[] = {
		libvlc_MediaDiscovererStarted,
		libvlc_MediaDiscovererEnded
	};
	nbEvents = sizeof(eventsMediaDiscoverer) / sizeof(*eventsMediaDiscoverer);
	for (int i = 0; i < nbEvents; i++) {
		p_libvlc_event_attach(_vlcMediaDiscovererEventManager, eventsMediaDiscoverer[i], libvlc_callback, this, _vlcException);
	}
	*/
}

void VLCMediaObject::libvlc_callback(const libvlc_event_t * event, void * user_data) {
	VLCMediaObject * vlcMediaObject = (VLCMediaObject *) user_data;

	qDebug() << (int) vlcMediaObject << "event:" << p_libvlc_event_type_name(event->type);

	/* Media player events */

	if (event->type == libvlc_MediaPlayerTimeChanged) {
		//new_time / VLC_POSITION_RESOLUTION since VLC adds * VLC_POSITION_RESOLUTION, don't know why...
		qDebug() << "new_time:" << event->u.media_player_time_changed.new_time;
		vlcMediaObject->totalTime();
		vlcMediaObject->currentTime();
		//emit vlcMediaObject->tick(event->u.media_instance_time_changed.new_time / VLC_POSITION_RESOLUTION);
		emit vlcMediaObject->tick(vlcMediaObject->currentTime());
		////////BUG POSITION//////////

		//Checks if the file is a video
		bool hasVideo = vlcMediaObject->hasVideo();
		if (hasVideo != vlcMediaObject->_hasVideo) {
			qDebug() << "libvlc_callback(): hasVideo:" << hasVideo;
			vlcMediaObject->_hasVideo = hasVideo;
			emit vlcMediaObject->hasVideoChanged(vlcMediaObject->_hasVideo);
		}

		//Checks if the file is seekable
		bool seekable = vlcMediaObject->isSeekable();
		if (seekable != vlcMediaObject->_seekable) {
			qDebug() << "libvlc_callback(): isSeekable:" << seekable;
			vlcMediaObject->_seekable = seekable;
			emit vlcMediaObject->seekableChanged(vlcMediaObject->_seekable);
		}
	}

	if (event->type == libvlc_MediaPlayerPlayed) {
		emit vlcMediaObject->stateChanged(Phonon::PlayingState);
	}

	if (event->type == libvlc_MediaPlayerPaused) {
		emit vlcMediaObject->stateChanged(Phonon::PausedState);
	}

	if (event->type == libvlc_MediaPlayerEndReached) {
		emit vlcMediaObject->stateChanged(Phonon::StoppedState);
		emit vlcMediaObject->finished();
	}

	if (event->type == libvlc_MediaPlayerStopped) {
		emit vlcMediaObject->stateChanged(Phonon::StoppedState);
	}

	/* Media events */

	if (event->type == libvlc_MediaDurationChanged) {
		//new_duration / VLC_POSITION_RESOLUTION since VLC adds * VLC_POSITION_RESOLUTION, don't know why...
		qDebug() << "new_duration:" << event->u.media_duration_changed.new_duration / 1000;

		//emit vlcMediaObject->totalTimeChanged(event->u.media_duration_changed.new_duration / 1000);

		//Checks if the file total time changed
		qint64 totalTime = vlcMediaObject->totalTime();
		if (totalTime != vlcMediaObject->_totalTime) {
			qDebug() << "libvlc_callback(): totalTime:" << totalTime;
			vlcMediaObject->_totalTime = totalTime;
			emit vlcMediaObject->totalTimeChanged(vlcMediaObject->_totalTime);
		}
	}

	if (event->type == libvlc_MediaMetaChanged) {
		QString meta = p_libvlc_media_get_meta(vlcMediaObject->_vlcMedia, event->u.media_meta_changed.meta_type, _vlcException);
		checkException();
		qDebug() << "libvlc_callback(): meta:" << meta;
	}
}

void VLCMediaObject::updateMetaData() {
	QMultiMap<QString, QString> metaDataMap;

	metaDataMap.insert(QLatin1String("ARTIST"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Artist, _vlcException)));
	checkException();
	metaDataMap.insert(QLatin1String("ALBUM"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Album, _vlcException)));
	checkException();
	metaDataMap.insert(QLatin1String("TITLE"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Title, _vlcException)));
	checkException();
	metaDataMap.insert(QLatin1String("DATE"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Date, _vlcException)));
	checkException();
	metaDataMap.insert(QLatin1String("GENRE"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Genre, _vlcException)));
	checkException();
	metaDataMap.insert(QLatin1String("TRACKNUMBER"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_TrackNumber, _vlcException)));
	checkException();
	metaDataMap.insert(QLatin1String("DESCRIPTION"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Description, _vlcException)));
	checkException();

	metaDataMap.insert(QLatin1String("COPYRIGHT"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_TrackNumber, _vlcException)));
	checkException();
	metaDataMap.insert(QLatin1String("URL"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_URL, _vlcException)));
	checkException();
	metaDataMap.insert(QLatin1String("ENCODEDBY"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_EncodedBy, _vlcException)));

	qDebug() << "updateMetaData(): artist:" << p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Artist, _vlcException);
	checkException();

	emit metaDataChanged(metaDataMap);

	emit stateChanged(Phonon::StoppedState);
}

qint64 VLCMediaObject::totalTime() const {
	libvlc_time_t time = p_libvlc_media_get_duration(_vlcMedia, _vlcException);
	checkException();

	time = time / VLC_POSITION_RESOLUTION;
	qDebug() << "totalTime:" << time;

	return time;
}

qint64 VLCMediaObject::currentTimeInternal() const {
	libvlc_time_t time = p_libvlc_media_player_get_time(_vlcMediaPlayer, _vlcException);
	checkException();

	qDebug() << "currentTime:" << time;

	return time;
}

}}	//Namespace Phonon::VLC_MPlayer
