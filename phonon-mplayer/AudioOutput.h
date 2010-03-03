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

#ifndef PHONON_MPLAYER_AUDIOOUTPUT_H
#define PHONON_MPLAYER_AUDIOOUTPUT_H

#include "SinkNode.h"

#include <phonon/audiooutputinterface.h>

namespace Phonon
{
namespace MPlayer
{

/**
 * MPlayer backend implementation of Phonon::AudioOutput.
 *
 * Allows to change volume and audio device.
 *
 * @author Tanguy Krotoff
 */
class AudioOutput : public SinkNode, public AudioOutputInterface {
	Q_OBJECT
	Q_INTERFACES(Phonon::AudioOutputInterface)
public:

	AudioOutput(QObject * parent);
	~AudioOutput();

	qreal volume() const;
	void setVolume(qreal volume);

	/** FIXME not implemented yet. */
	int outputDevice() const;

	/** FIXME not implemented yet. */
	bool setOutputDevice(int device);

	/** FIXME not implemented yet. */
	bool setOutputDevice(const Phonon::AudioOutputDevice & device);

signals:

	void volumeChanged(qreal volume);

	/** FIXME not implemented yet. */
	void audioDeviceFailed();

private:

};

}}	//Namespace Phonon::MPlayer

#endif	//PHONON_MPLAYER_AUDIOOUTPUT_H
