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

#ifndef PLAYLISTPARSER_H
#define PLAYLISTPARSER_H

#include <playlistparser/playlistparser_export.h>
#include <playlistparser/IPlaylistParser.h>

#include <QtCore/QList>

/**
 * Parses a playlist file.
 *
 * A playlist file can be a .m3u, m3u8, .pls, .wpl...
 * Prefered format is m3u "The most popular format, M3U, is also the simplest."
 *
 * @see http://en.wikipedia.org/wiki/M3u
 * @see http://gonze.com/playlists/playlist-format-survey.html
 * @author Tanguy Krotoff
 */
class PLAYLISTPARSER_API PlaylistParser : public IPlaylistParser {
public:

	PlaylistParser(const QString & filename);

	~PlaylistParser();

	QStringList fileExtensions() const;

	void load();

	bool save(const QStringList & files);

private:

	/** List of all available parsers. */
	QList<IPlaylistParser *> _parserList;

	/** Current selected parser. */
	IPlaylistParser * _parser;
};

#endif	//PLAYLISTPARSER_H
