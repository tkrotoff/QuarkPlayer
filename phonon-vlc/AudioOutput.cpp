/*
 * VLC backend for the Phonon library
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

#include "vlc_loader.h"
#include "vlc_symbols.h"

namespace Phonon
{
namespace VLC
{

AudioOutput::AudioOutput(QObject * parent)
	: QObject(parent) {
}

AudioOutput::~AudioOutput() {
}

qreal AudioOutput::volume() const {
	qreal volume = 0;

	//FIXME we have to check first for _vlcCurrentMediaPlayer
	//libvlc_audio_get_volume() does not work otherwise
	if (_vlcCurrentMediaPlayer) {
		volume = p_libvlc_audio_get_volume(_vlcInstance, _vlcException) / 100;
		checkException();
	}
	return volume;
}

void AudioOutput::setVolume(qreal volume) {
	if (_vlcInstance) {
		p_libvlc_audio_set_volume(_vlcInstance, volume * 100, _vlcException);
		checkException();
	}
}

int AudioOutput::outputDevice() const {
	return 0;
}

bool AudioOutput::setOutputDevice(int device) {
	return true;
}

}}	//Namespace Phonon::VLC
