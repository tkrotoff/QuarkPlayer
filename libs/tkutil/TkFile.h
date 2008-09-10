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

#ifndef TKFILE_H
#define TKFILE_H

#include <tkutil/tkutil_export.h>

#include <QtCore/QString>

/**
 * Useful functions for QFile.
 *
 * This has been reimplemented since QFile and QFileInfo is pretty slow.
 *
 * @see QFile
 * @see QFileInfo
 * @author Tanguy Krotoff
 */
class TKUTIL_API TkFile {
public:

	/**
	 * Gets the filename part from a path.
	 *
	 * Example: path=/usr/lib/toto.txt filename=toto.txt
	 */
	static QString fileName(const QString & path);

	/**
	 * Gets the directory name that contains the given file.
	 *
	 * Example: filename=/usr/lib/toto.txt dir=lib
	 */
	static QString dir(const QString & filename);

	/**
	 * Removes the extension (i.e .mp3) from the filename.
	 *
	 * Example: path=/usr/lib/toto.txt => /usr/lib/toto
	 */
	static QString removeFileExtension(const QString & path);

	/**
	 * Gets the extension (i.e mp3) from the filename.
	 *
	 * Example: path=/usr/lib/toto.txt => txt
	 */
	static QString fileExtension(const QString & path);

	/**
	 * Gets the path of a file.
	 *
	 * Example: filename=/toto/titi/caca.txt path=/toto/titi
	 */
	static QString path(const QString & filename);

	/**
	 * Checks if a path is a directory or a file.
	 *
	 * @return true if it is a path; false otherwise
	 */
	static bool isDir(const QString & path);

private:

	TkFile();
};

#endif	//TKFILE_H
