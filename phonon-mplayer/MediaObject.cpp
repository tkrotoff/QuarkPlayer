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

namespace Phonon
{
namespace MPlayer
{

MediaObject::MediaObject(QObject * parent)
	: QObject(parent) {

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
	case MediaSource::LocalFile: {
		//FIXME Crash if not done using new QString()
		QString * filename = new QString(_mediaSource.fileName());
		Backend::getSMPlayerCore()->openFile(*filename);
		break;
	}
	case MediaSource::Url:
		break;
	case MediaSource::Disc: {
		switch (_mediaSource.discType()) {
		case Phonon::NoDisc:
			//kFatal(610) << "I should never get to see a MediaSource that is a disc but doesn't specify which one";
			return;
		case Phonon::Cd:
			break;
		case Phonon::Dvd:
			break;
		case Phonon::Vcd:
			break;
		default:
			return;
		}
		break;
	}
	case MediaSource::Stream:
		break;
	}
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
		break;
	case MediaSource::Url:
		break;
	case MediaSource::Disc: {
		switch (source.discType()) {
		case Phonon::NoDisc:
			//kFatal(610) << "I should never get to see a MediaSource that is a disc but doesn't specify which one";
			return;
		case Phonon::Cd:
			break;
		case Phonon::Dvd:
			break;
		case Phonon::Vcd:
			break;
		default:
			return;
		}
		}
		break;
	case MediaSource::Stream:
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
	Phonon::State previousState = _currentState;

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
		_currentState = Phonon::StoppedState;
		_currentTime = 0;
		break;
	case Core::Playing:
		_currentState = Phonon::PlayingState;
		emit totalTimeChanged(totalTime());
		emit seekableChanged(true);
		break;
	case Core::Paused:
		_currentState = Phonon::PausedState;
		break;
	}

	qDebug() << __FUNCTION__ << "currentState:" << _currentState << "previousState:" << previousState;

	emit stateChanged(_currentState, previousState);
}

bool MediaObject::hasInterface(Interface iface) const {
	return true;
}

QVariant MediaObject::interfaceCall(Interface iface, int command, const QList<QVariant> & arguments) {
	return new QVariant();
}

}}	//Namespace Phonon::MPlayer
