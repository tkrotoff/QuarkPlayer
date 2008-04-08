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

#include "MediaObject.h"

#include "Backend.h"

#include "smplayer/core.h"
#include "smplayer/infoprovider.h"

#include <QtCore/QUrl>

namespace Phonon
{
namespace MPlayer
{

MediaObject::MediaObject(QObject * parent)
	: QObject(parent) {

	_isPlaying = false;
	_currentTime = 0;
	_currentState = Phonon::LoadingState;

	connect(Backend::getSMPlayerCore(), SIGNAL(showTime(double)),
		SLOT(tickSlotInternal(double)));
	connect(Backend::getSMPlayerCore(), SIGNAL(stateChanged(Core::State)),
		SLOT(stateChangedSlotInternal(Core::State)));
}

MediaObject::~MediaObject() {
}

void MediaObject::play() {
	qDebug() << "MediaObject::play()";

	switch (_mediaSource.type()) {

	case MediaSource::Invalid:
		break;

	case MediaSource::LocalFile:
		playInternal(_mediaSource.fileName());
		break;

	case MediaSource::Url:
		playInternal(_mediaSource.url().toString());
		break;

	case MediaSource::Disc: {
		switch (_mediaSource.discType()) {
		case Phonon::NoDisc:
			//kFatal(610) << "I should never get to see a MediaSource that is a disc but doesn't specify which one";
			return;
		case Phonon::Cd:
			playInternal(_mediaSource.deviceName());
			break;
		case Phonon::Dvd:
			playInternal(_mediaSource.deviceName());
			break;
		case Phonon::Vcd:
			playInternal(_mediaSource.deviceName());
			break;
		default:
			qCritical() << __FUNCTION__ << "error: unsupported MediaSource::Disc:" << _mediaSource.discType();
		}
		break;
	}

	case MediaSource::Stream:
		break;

	default:
		qCritical() << __FUNCTION__ << "error: unsupported MediaSource:" << _mediaSource.type();
		break;

	}
}

void MediaObject::loadMediaInternal(const QString & filename) {
	MediaData mediaData = InfoProvider::getInfo(filename);

	QMultiMap<QString, QString> metaDataMap;
	metaDataMap.insert(QLatin1String("ARTIST"), mediaData.clip_artist);
	metaDataMap.insert(QLatin1String("ALBUM"), mediaData.clip_album);
	metaDataMap.insert(QLatin1String("TITLE"), mediaData.clip_name);
	metaDataMap.insert(QLatin1String("DATE"), mediaData.clip_date);
	metaDataMap.insert(QLatin1String("GENRE"), mediaData.clip_genre);
	metaDataMap.insert(QLatin1String("TRACKNUMBER"), mediaData.clip_track);
	metaDataMap.insert(QLatin1String("DESCRIPTION"), mediaData.clip_comment);
	metaDataMap.insert(QLatin1String("COPYRIGHT"), mediaData.clip_copyright);
	metaDataMap.insert(QLatin1String("URL"), mediaData.stream_url);
	metaDataMap.insert(QLatin1String("ENCODEDBY"), mediaData.clip_software);
	emit metaDataChanged(metaDataMap);

	emit totalTimeChanged(mediaData.duration);

	emit hasVideoChanged(!mediaData.novideo);

	stateChangedInternal(Phonon::StoppedState);
}

void MediaObject::playInternal(const QString & filename) {
	_isPlaying = true;
	Backend::getSMPlayerCore()->open(filename);
}

void MediaObject::pause() {
	Backend::getSMPlayerCore()->pause();
}

void MediaObject::stop() {
	Backend::getSMPlayerCore()->stop();
}

void MediaObject::seek(qint64 milliseconds) {
	qDebug() << __FUNCTION__ << milliseconds / 1000.0;

	//Core::seek() works using seconds
	Backend::getSMPlayerCore()->seek(milliseconds / 1000.0);
}

qint32 MediaObject::tickInterval() const {
	//Tick interval is 1 second = 1000 milliseconds
	return 1000;
}

void MediaObject::setTickInterval(qint32 interval) {
	//Tick interval is 1 second = 1000 milliseconds
}

bool MediaObject::hasVideo() const {
	return true;
}

bool MediaObject::isSeekable() const {
	return true;
}

qint64 MediaObject::currentTime() const {
	return _currentTime;
}

Phonon::State MediaObject::state() const {
	return _currentState;
}

QString MediaObject::errorString() const {
	return "";
}

Phonon::ErrorType MediaObject::errorType() const {
	return Phonon::NormalError;
}

qint64 MediaObject::totalTime() const {
	qDebug() << __FUNCTION__ << "duration:" << Backend::getSMPlayerCore()->mdat.duration;
	return Backend::getSMPlayerCore()->mdat.duration * 1000;
}

MediaSource MediaObject::source() const {
	return _mediaSource;
}

void MediaObject::setSource(const MediaSource & source) {
	_mediaSource = source;

	switch (source.type()) {
	case MediaSource::Invalid:
		break;
	case MediaSource::LocalFile:
		loadMediaInternal(_mediaSource.fileName());
		break;
	case MediaSource::Url:
		loadMediaInternal(_mediaSource.url().toString());
		break;
	case MediaSource::Disc: {
		switch (source.discType()) {
		case Phonon::NoDisc:
			qCritical() << __FUNCTION__ << "error: the MediaSource::Disc doesn't specify which one (Phonon::NoDisc)";
			return;
		case Phonon::Cd:
			loadMediaInternal(_mediaSource.deviceName());
			break;
		case Phonon::Dvd:
			loadMediaInternal(_mediaSource.deviceName());
			break;
		case Phonon::Vcd:
			loadMediaInternal(_mediaSource.deviceName());
			break;
		default:
			qCritical() << __FUNCTION__ << "error: unsupported MediaSource::Disc:" << source.discType();
			break;
		}
		}
		break;
	case MediaSource::Stream:
		break;
	default:
		qCritical() << __FUNCTION__ << "error: unsupported MediaSource:" << source.type();
		break;
	}
}

void MediaObject::setNextSource(const MediaSource & source) {
	_mediaSource = source;
}

qint32 MediaObject::prefinishMark() const {
	return 0;
}

void MediaObject::setPrefinishMark(qint32) {
}

qint32 MediaObject::transitionTime() const {
	return 0;
}

void MediaObject::setTransitionTime(qint32) {
}

void MediaObject::tickSlotInternal(double seconds) {
	qDebug() << __FUNCTION__ << seconds;

	//time is in milliseconds
	_currentTime = seconds * 1000;
	emit tick(_currentTime);
}

void MediaObject::stateChangedSlotInternal(Core::State newState) {
	if (!_isPlaying) {
		return;
	}

	/*
	Phonon::LoadingState
	Phonon::StoppedState
	Phonon::PlayingState
	Phonon::BufferingState
	Phonon::PausedState
	Phonon::ErrorState
	*/

	switch(newState) {
	case Core::Stopped:
		stateChangedInternal(Phonon::StoppedState);
		_currentTime = 0;
		break;
	case Core::Playing:
		stateChangedInternal(Phonon::PlayingState);
		emit totalTimeChanged(totalTime());
		emit seekableChanged(true);
		break;
	case Core::Paused:
		stateChangedInternal(Phonon::PausedState);
		break;
	}
}

void MediaObject::stateChangedInternal(Phonon::State newState) {
	if (newState == _currentState) {
		//No state changed
		return;
	}

	//State changed
	Phonon::State previousState = _currentState;
	_currentState = newState;
	emit stateChanged(_currentState, previousState);
}

bool MediaObject::hasInterface(Interface iface) const {
	return true;
}

QVariant MediaObject::interfaceCall(Interface iface, int command, const QList<QVariant> & arguments) {
	return new QVariant();
}

}}	//Namespace Phonon::MPlayer
