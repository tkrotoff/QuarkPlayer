/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef UTIL_H
#define UTIL_H

class QString;

/**
 * Utility functions for the different parsers.
 *
 * @author Tanguy Krotoff
 */
class Util {
public:

	static QString pathToNativeSeparators(const QString & path);

	static QString canonicalFilePath(const QString & path, const QString & filename);

	static QString relativeFilePath(const QString & path, const QString & filename);

private:

	Util();

	~Util();
};

#endif	//UTIL_H
