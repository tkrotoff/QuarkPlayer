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

#include "AudioOutput.h"

#include "MediaObject.h"

#ifdef PHONON_VLC
	#include "VLCMediaObject.h"

	#include "vlc_loader.h"
	#include "vlc_symbols.h"
#endif	//PHONON_VLC

#ifdef PHONON_MPLAYER
	#include "MPlayerMediaObject.h"

	#include <mplayer/MPlayerProcess.h>
#endif	//PHONON_MPLAYER

namespace Phonon
{
namespace VLC_MPlayer
{

AudioOutput::AudioOutput(QObject * parent)
	: QObject(parent) {

	_mediaObject = NULL;
}

AudioOutput::~AudioOutput() {
}

void AudioOutput::connectToMediaObject(MediaObject * mediaObject) {
	if (_mediaObject && mediaObject) {
		qCritical() << __FUNCTION__ << "_mediaObject already connected";
	}

	_mediaObject = mediaObject;
}

void AudioOutput::sendMPlayerCommand(const QString & command) const {
#ifdef PHONON_MPLAYER
	if (_mediaObject) {
		MPlayerProcess * process = _mediaObject->getPrivateMediaObject().getMPlayerProcess();
		if (process) {
			process->writeToStdin(command);
		}
	}
#endif	//PHONON_MPLAYER
}

qreal AudioOutput::volume() const {
	qreal volume = 0;

#ifdef PHONON_VLC
	if (_vlcCurrentMediaPlayer) {
		volume = p_libvlc_audio_get_volume(_vlcInstance, _vlcException) / 100;
		checkException();
	}
#endif	//PHONON_VLC

	sendMPlayerCommand("get_property volume");

	return volume;
}

void AudioOutput::setVolume(qreal volume) {
#ifdef PHONON_VLC
	if (_vlcInstance) {
		p_libvlc_audio_set_volume(_vlcInstance, volume * 100, _vlcException);
		checkException();
	}
#endif	//PHONON_VLC

	sendMPlayerCommand("volume " + QString::number(volume * 100) + " 1");
}

int AudioOutput::outputDevice() const {
	return 0;
}

bool AudioOutput::setOutputDevice(int device) {
	return true;
}

}}	//Namespace Phonon::VLC_MPlayer
