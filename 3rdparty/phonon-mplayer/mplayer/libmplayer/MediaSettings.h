/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
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

#ifndef PHONON_MPLAYER_MEDIASETTINGS_H
#define PHONON_MPLAYER_MEDIASETTINGS_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSize>

namespace Phonon
{
namespace MPlayer
{

/**
 * Settings for the MPlayer process.
 *
 * MPlayer uses command line arguments.
 * Here are some of the arguments that will be given to MPlayer command line.
 *
 * Read by class MPlayerLoader written by classes MediaObject, Effect and AudioOutput.
 * This class allows to pass parameters to the class MPlayerLoader (which contains a static
 * instance of this class).
 *
 * @see MPlayerLoader
 * @author Tanguy Krotoff
 */
class MediaSettings {
public:

	MediaSettings();
	~MediaSettings();

	void clear();

	/**
	 * MPlayer volume, see MPlayer manpage.
	 *
	 * -volume <-1-100> (also see -af volume)
	 * Sets the startup volume in the mixer, either hardware or software (if used with -softvol).
	 * A value of -1 (the default) will not change the volume.
	 */
	int volume;

	/** Video contrast. */
	int contrast;

	/** Video brightness. */
	int brightness;

	/** Video hue. */
	int hue;

	/** Video saturation. */
	int saturation;

	/**
	 * Audio filters.
	 *
	 * - "karaoke"
	 * - "extrastereo"
	 * - "volnorm=2"
	 */
	QStringList audioFilters;

	/**
	 * Video filters.
	 */
	QStringList videoFilters;

	/**
	 * Optial device name for MPlayer.
	 *
	 * -dvd-device argument for MPlayer
	 * <pre>mplayer dvd://title -dvd-device d:</pre>
	 * <pre>mplayer dvd://1 -dvd-device /dev/dvd</pre>
	 *
	 * -cdrom-device argument for MPlayer
	 * <pre>mplayer vcd://track -cdrom-device d:</pre>
	 * <pre>mplayer vcd://1 -cdrom-device /dev/cdrom</pre>
	 * <pre>mplayer cdda://1 -cdrom-device /dev/cdrom</pre>
	 */
	QString opticalDeviceName;
};

}}	//Namespace Phonon::MPlayer

#endif	//PHONON_MPLAYER_MEDIASETTINGS_H
