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

#ifndef RANDOM_H
#define RANDOM_H

#include <tkutil/tkutil_export.h>

#include <QtCore/QString>
#include <QtCore/QList>

/**
 * Functions to generate random numbers.
 *
 * @author Tanguy Krotoff
 */
class TKUTIL_API Random {
public:

	template <class T>
	static QList<T> & randomize(QList<T> & list) {
		int min = 0;
		int max = list.size();

		for (int i = 0; i < max; i++) {
			int randomIndex1 = randomInt(min, max - 1);
			int randomIndex2 = randomInt(min, max - 1);
			list.swap(randomIndex1, randomIndex2);
		}

		return list;
	}

	/**
	 * Generates a random number between min (included) and max (included).
	 *
	 * Will never give you the 2 same numbers in a row (i.e 3,3; 3,5,3 however is ok)
	 *
	 * @see http://www.geekpedia.com/tutorial39_Random-Number-Generation.html
	 */
	static int randomInt(int min, int max);

private:

	Random();
};

#endif	//RANDOM_H
