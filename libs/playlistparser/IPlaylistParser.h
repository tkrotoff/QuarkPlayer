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

#ifndef IPLAYLISTPARSER_H
#define IPLAYLISTPARSER_H

class QString;
class QStringList;

/**
 * Interface for implementing playlist file format parser.
 *
 * @author Tanguy Krotoff
 */
class IPlaylistParser {
public:

	/**
	 * Parses (load/save) a given playlist file.
	 *
	 * @param filename playlist file (full path) to parse.
	 */
	IPlaylistParser(const QString & filename) { }

	virtual ~IPlaylistParser() { }

	/**
	 * Returns the list of file extensions supported by this parser.
	 *
	 * @return list of file extensions (i.e m3u, pls...)
	 */
	virtual QStringList fileExtensions() const = 0;

	/**
	 * Loads the playlist.
	 *
	 * @return list of files (full path filename) contained inside the playlist
	 */
	virtual QStringList load() = 0;

	/**
	 * Saves the playlist.
	 *
	 * @param files files to add to the playlist file
	 * @return true if everything Ok; false otherwise
	 */
	virtual bool save(const QStringList & files) = 0;
};

#endif	//IPLAYLISTPARSER_H
