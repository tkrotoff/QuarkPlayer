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

#ifndef IPLAYLISTPARSER_H
#define IPLAYLISTPARSER_H

#include <PlaylistParser/PlaylistParserExport.h>

#include <QtCore/QObject>

/**
 * Interface for implementing playlist file format parser.
 *
 * Follows the same API as TkUtil::FindFiles class.
 *
 * @see FindFiles
 * @author Tanguy Krotoff
 */
class PLAYLISTPARSER_API IPlaylistParser : public QObject {
public:

	/**
	 * Parses a given playlist.
	 *
	 * You must do a <pre>new PlaylistParser</pre> since PlaylistParser is threaded
	 * and sends signals.
	 */
	IPlaylistParser(QObject * parent);

	virtual ~IPlaylistParser();

	/**
	 * Stops the thread.
	 */
	virtual void stop() = 0;
};

#endif	//IPLAYLISTPARSER_H
