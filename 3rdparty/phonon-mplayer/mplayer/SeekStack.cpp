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

#include "SeekStack.h"

#include "PhononMPlayerLogger.h"

#include <QtCore/QTimer>

namespace Phonon
{
namespace MPlayer
{

SeekStack::SeekStack(MediaObject * mediaObject)
	: QObject(mediaObject) {

	_mediaObject = mediaObject;

	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), SLOT(popSeek()));
	_timer->setInterval(1000);
}

SeekStack::~SeekStack() {
}

void SeekStack::pushSeek(qint64 milliseconds) {
	PhononMPlayerDebug() << "Seek:" << milliseconds;

	disconnect(_mediaObject->getMPlayerProcess(), SIGNAL(tick(qint64)),
		_mediaObject, SLOT(tickInternal(qint64)));

	_stack.push(milliseconds);

	if (!_timer->isActive()) {
		_timer->start();
		popSeek();
	}
}

void SeekStack::popSeek() {
	if (_stack.isEmpty()) {
		_timer->stop();
		reconnectTickSignal();
		return;
	}

	int milliseconds = _stack.pop();
	_stack.clear();

	PhononMPlayerDebug() << "Real seek:" << milliseconds;

	_mediaObject->seekInternal(milliseconds);

	//MPlayer takes some time before to send back the proper current time
	QTimer::singleShot(200, this, SLOT(reconnectTickSignal()));
}

void SeekStack::reconnectTickSignal() {
	connect(_mediaObject->getMPlayerProcess(), SIGNAL(tick(qint64)),
		_mediaObject, SLOT(tickInternal(qint64)));
}

}}	//Namespace Phonon::MPlayer
