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

#include "MediaObject.h"

#include <QtCore/QUrl>
#include <QtCore/QMetaType>
#include <QtCore/QTimer>

namespace Phonon
{
namespace VLC_MPlayer
{

MediaObject::MediaObject(QObject * parent)
	: QObject(parent) {

	_currentState = Phonon::LoadingState;
	_videoWidgetId = 0;

	qRegisterMetaType<QMultiMap<QString, QString> >("QMultiMap<QString, QString>");

	connect(this, SIGNAL(stateChanged(Phonon::State)),
		SLOT(stateChangedInternal(Phonon::State)));
}

MediaObject::~MediaObject() {
	stop();
}

void MediaObject::setVideoWidgetId(int videoWidgetId) {
	_videoWidgetId = videoWidgetId;
}

void MediaObject::play() {
	qDebug() << __FUNCTION__;

	if (_currentState == Phonon::PausedState) {
		resume();
	} else {
		//Play the file
		playInternal();
	}
}

void MediaObject::loadMedia(const QString & filename) {
	//Default MediaObject state is Phonon::LoadingState
	_currentState = Phonon::LoadingState;

	//Loads the media
	loadMediaInternal(filename);
}

void MediaObject::resume() {
	pause();
}

void MediaObject::stop() {
	Phonon::State st = state();
	if (st == Phonon::PlayingState || st == Phonon::PausedState) {
		stopInternal();
	}
}

qint32 MediaObject::tickInterval() const {
	return 1000;
}

void MediaObject::setTickInterval(qint32 interval) {
}

qint64 MediaObject::currentTime() const {
	qint64 time = -1;
	Phonon::State st = state();

	switch(st) {
	case Phonon::PausedState:
		time = currentTimeInternal();
		break;
	case Phonon::BufferingState:
		time = currentTimeInternal();
		break;
	case Phonon::PlayingState:
		time = currentTimeInternal();
		break;
	case Phonon::StoppedState:
		time = 0;
		break;
	case Phonon::LoadingState:
		time = 0;
		break;
	case Phonon::ErrorState:
		time = -1;
		break;
	default:
		qCritical() << __FUNCTION__ << "Error: unsupported Phonon::State:" << st;
	}

	return time;
}

Phonon::State MediaObject::state() const {
	return _currentState;
}

Phonon::ErrorType MediaObject::errorType() const {
	return Phonon::NormalError;
}

MediaSource MediaObject::source() const {
	return _mediaSource;
}

void MediaObject::setSource(const MediaSource & source) {
	qDebug() << __FUNCTION__;

	_mediaSource = source;

	switch (source.type()) {
	case MediaSource::Invalid:
		break;
	case MediaSource::LocalFile:
		loadMedia(_mediaSource.fileName());
		break;
	case MediaSource::Url:
		loadMedia(_mediaSource.url().toString());
		break;
	case MediaSource::Disc: {
		switch (source.discType()) {
		case Phonon::NoDisc:
			qCritical() << __FUNCTION__ << "Error: the MediaSource::Disc doesn't specify which one (Phonon::NoDisc)";
			return;
		case Phonon::Cd:
			loadMedia(_mediaSource.deviceName());
			break;
		case Phonon::Dvd:

#ifdef PHONON_VLC
			loadMedia(_mediaSource.deviceName());
#endif	//PHONON_VLC

#ifdef PHONON_MPLAYER
			loadMedia("dvd://");
#endif	//PHONON_MPLAYER

			break;
		case Phonon::Vcd:
			loadMedia(_mediaSource.deviceName());
			break;
		default:
			qCritical() << __FUNCTION__ << "Error: unsupported MediaSource::Disc:" << source.discType();
			break;
		}
		}
		break;
	case MediaSource::Stream:
		break;
	default:
		qCritical() << __FUNCTION__ << "Error: unsupported MediaSource:" << source.type();
		break;
	}
}

void MediaObject::setNextSource(const MediaSource & source) {
	setSource(source);
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

void MediaObject::stateChangedInternal(Phonon::State newState) {
	qDebug() << __FUNCTION__ << "previousState:" << _currentState << "newState:" << newState;

	if (newState == _currentState) {
		//No state changed
		return;
	}

	//State changed
	Phonon::State previousState = _currentState;
	_currentState = newState;
	emit stateChanged(_currentState, previousState);
}

}}	//Namespace Phonon::VLC_MPlayer
