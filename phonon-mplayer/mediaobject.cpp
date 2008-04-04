/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mediaobject.h"

#include "backend.h"

#include "smplayer/core.h"

namespace Phonon
{
namespace MPlayer
{

MediaObject::MediaObject(QObject * parent)
	: QObject(parent) {

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
	//Core::seek() works using seconds
	Backend::getSMPlayerCore()->seek(milliseconds * 1000);
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
	return 0;
}

Phonon::State MediaObject::state() const {
	return Phonon::StoppedState;
}

QString MediaObject::errorString() const {
	return "";
}

Phonon::ErrorType MediaObject::errorType() const {
	return Phonon::NormalError;
}

qint64 MediaObject::totalTime() const {
	return 0;
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
	//time is in milliseconds
	qint64 time = seconds * 1000;
	emit tick(time);
}

void MediaObject::stateChangedSlotInternal(Core::State state) {
	/*
	Phonon::LoadingState
	Phonon::StoppedState
	Phonon::PlayingState
	Phonon::BufferingState
	Phonon::PausedState
	Phonon::ErrorState
	*/

	switch(state) {
	case Core::Stopped:
		emit stateChanged(Phonon::StoppedState, Phonon::ErrorState);
		break;
	case Core::Playing:
		emit stateChanged(Phonon::PlayingState, Phonon::ErrorState);
		break;
	case Core::Paused:
		emit stateChanged(Phonon::PausedState, Phonon::ErrorState);
		break;
	}
}

bool MediaObject::hasInterface(Interface iface) const {
	return true;
}

QVariant MediaObject::interfaceCall(Interface iface, int command, const QList<QVariant> & arguments) {
	return new QVariant();
}

}}	//Namespace Phonon::MPlayer
