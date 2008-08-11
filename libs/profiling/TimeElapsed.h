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

#ifndef TIMEELAPSED_H
#define TIMEELAPSED_H

#include <QtCore/QTime>

/**
 * Compute time elasped of a function.
 *
 * Example:
 * <pre>
 * void toto() {
 *     TimeElapsed time;
 *     some_lengthy_task();
 *     //TimeElapsed destructor called,
 *     //shows the time elapsed by the function
 * }
 *
 * @author Tanguy Krotoff
 */
class TimeElapsed {
public:

	TimeElapsed();

	~TimeElapsed();

private:

	QTime _time;
};

#endif	//TIMEELAPSED_H
