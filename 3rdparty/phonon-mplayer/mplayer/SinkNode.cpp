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

#include "SinkNode.h"

#include "MediaObject.h"
#include "PhononMPlayerLogger.h"

#include "libmplayer/MPlayerProcess.h"

namespace Phonon
{
namespace MPlayer
{

SinkNode::SinkNode(QObject * parent)
	: QObject(parent) {

	_mediaObject = NULL;
}

SinkNode::~SinkNode() {
}

void SinkNode::connectToMediaObject(MediaObject * mediaObject) {
	if (_mediaObject) {
		PhononMPlayerCritical() << "_mediaObject already connected";
	}

	_mediaObject = mediaObject;
}

void SinkNode::disconnectFromMediaObject(MediaObject * mediaObject) {
	if (_mediaObject != mediaObject) {
		PhononMPlayerCritical() << "SinkNode was not connected to mediaObject";
	}
}

bool SinkNode::sendMPlayerCommand(const QString & command) const {
	bool result = false;

	if (_mediaObject) {
		MPlayerProcess * process = _mediaObject->getMPlayerProcess();
		if (process->isRunning()) {
			result = process->sendCommand(command);
		} else {
			//FIXME Make it silent?
			PhononMPlayerWarning() << "MPlayer process not running";
		}
	}

	return result;
}

}}	//Namespace Phonon::MPlayer
