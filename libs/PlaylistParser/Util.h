/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef UTIL_H
#define UTIL_H

class QString;
class QIODevice;

/**
 * Utility functions for the different parsers.
 *
 * @author Tanguy Krotoff
 */
class Util {
public:

	/** Converts every / or \ to native separators. */
	static QString pathToNativeSeparators(const QString & path);

	/**
	 * Returns the canonical path including the file name.
	 *
	 * A canonical path is an absolute path without symbolic links or redundant "." or ".." elements.
	 *
	 * If the file does not exist, canonicalFilePath() returns an empty string.
	 *
	 * @see QFileInfo::canonicalFilePath()
	 */
	static QString canonicalFilePath(const QString & path, const QString & fileName);

	/**
	 * Opens the location (filename) in read mode.
	 *
	 * Code factorization.
	 * If the QIODevice returned != NULL then it should be deleted by you.
	 *
	 * @param location location (filename) to open
	 * @return QIODevice or NULL
	 */
	static QIODevice * openLocationReadMode(const QString & location);

	/**
	 * Opens the location (filename) in write mode.
	 *
	 * Code factorization.
	 * If the QIODevice returned != NULL then it should be deleted by you.
	 *
	 * @param location location (filename) to open
	 * @return QIODevice or NULL
	 */
	static QIODevice * openLocationWriteMode(const QString & location);

private:

	Util();

	~Util();
};

#endif	//UTIL_H
