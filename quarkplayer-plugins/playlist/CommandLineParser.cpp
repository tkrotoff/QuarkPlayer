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

#include "CommandLineParser.h"

#include "PlaylistModel.h"

#include <QtCore/QUrl>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

CommandLineParser::CommandLineParser(PlaylistModel * playlistModel) {
	_playlistModel = playlistModel;

	start();
}

CommandLineParser::~CommandLineParser() {
}

void CommandLineParser::start() {
	QStringList args(QCoreApplication::arguments());

	//Delete the first one in the list,
	//it is the name of the application
	//we don't care about this
	args.removeAt(0);

	bool playlistEnqueue = false;
	QStringList files;
	QStringList playlists;
	foreach (QString arg, args) {
		if (arg == "--playlist-enqueue") {
			playlistEnqueue = true;
		} else if (QFileInfo(arg).exists()) {
			//This is a real file or directory:
			//let's add it to the playlist
			files += arg;
		} else if (QUrl(arg).isValid()) {
			//This is a real URL:
			//let's add it to the playlist
			files += arg;
		}
	}

	if (!files.isEmpty()) {
		_playlistModel->clear();
		_playlistModel->addFiles(files);

		if (!playlistEnqueue) {
			//Plays the first valid file/URL found from the command line arguments
			_playlistModel->play(0);
		}
	} else {
		//Normal start, loads the last playlist used
		_playlistModel->loadCurrentPlaylist();
	}
}
