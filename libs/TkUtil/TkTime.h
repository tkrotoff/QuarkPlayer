/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef TKTIME_H
#define TKTIME_H

#include <tkutil/tkutil_export.h>

/**
 * Time convertion functions.
 *
 * @author Tanguy Krotoff
 */
class TKUTIL_API TkTime {
public:

	~TkTime();

	/** Converts from milliseconds to a string like hh:mm:ss or mm:ss depending if hour > 0. */
	static QString convertMilliseconds(qint64 milliseconds);

	/** Converts from seconds to a string like hh:mm:ss or mm:ss depending if hour > 0. */
	static QString convertSeconds(int seconds);

	/** Converts from milliseconds to a string like hh:mm:ss / hh:mm:ss or mm:ss / mm:ss depending if hour > 0. */
	static QString convertMilliseconds(qint64 currentTime, qint64 totalTime);

private:

	TkTime();
};

#endif	//TKTIME_H
