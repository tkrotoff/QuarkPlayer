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

#include "Random.h"

#include <QtCore/QtGlobal>
#include <QtCore/QDebug>

#include <ctime>

int Random::randomInt(int min, int max) {
	static bool firstTime = true;
	if (firstTime) {
		firstTime = false;
		qsrand(time(NULL));
	}

	int number = 0;

	if (min > max) {
		number = max + (int) (qrand() * (min - max + 1) / (RAND_MAX + 1.0));
	} else {
		number = min + (int) (qrand() * (max - min + 1) / (RAND_MAX + 1.0));
	}

	qDebug() << __FUNCTION__ << number;

	return number;
}
