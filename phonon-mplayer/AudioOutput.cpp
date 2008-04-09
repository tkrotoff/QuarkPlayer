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

#include "AudioOutput.h"

#include <mplayer/MPlayerProcess.h>
#include <mplayer/MPlayerLoader.h>

namespace Phonon
{
namespace MPlayer
{

AudioOutput::AudioOutput(QObject * parent)
	: QObject(parent) {

	_volume = 0;
}

AudioOutput::~AudioOutput() {
}

qreal AudioOutput::volume() const {
	MPlayerProcess * process = MPlayerLoader::getCurrentMPlayerProcess();
	if (process) {
		process->writeToStdin("	get_property volume");
	}

	return _volume;
}

void AudioOutput::setVolume(qreal volume) {
	MPlayerProcess * process = MPlayerLoader::getCurrentMPlayerProcess();
	if (process) {
		process->writeToStdin("volume " + QString::number(volume * 100) + " 1");
	}
	AudioOutput::volume();
}

int AudioOutput::outputDevice() const {
	return 0;
}

bool AudioOutput::setOutputDevice(int device) {
	return true;
}

}}	//Namespace Phonon::MPlayer
