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

#ifndef PLAYLISTPARSERTHREAD_H
#define PLAYLISTPARSERTHREAD_H

#include <QtCore/QThread>
#include <QtCore/QStringList>

class IPlaylistParser;

/**
 *
 * @author Tanguy Krotoff
 */
class PlaylistParserThread : public QThread {
	Q_OBJECT
public:

	PlaylistParserThread(IPlaylistParser * parser);

	~PlaylistParserThread();

	void load();

	void save(const QStringList & files);

	void stop();

private:

	void run();

	IPlaylistParser * _parser;

	QStringList _files;

	bool _save;
	bool _load;
};

#endif	//PLAYLISTPARSERTHREAD_H
