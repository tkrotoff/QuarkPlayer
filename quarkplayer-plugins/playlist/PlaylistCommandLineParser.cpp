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

#include "PlaylistCommandLineParser.h"

#include "PlaylistModel.h"

#include <quarkplayer/CommandLineManager.h>

#include <QtCore/QUrl>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

PlaylistCommandLineParser::PlaylistCommandLineParser(PlaylistModel * playlistModel)
	: ICommandLineParser() {

	_playlistModel = playlistModel;

	CommandLineManager::instance().addCommandLineParser(this);
}

PlaylistCommandLineParser::~PlaylistCommandLineParser() {
	CommandLineManager::instance().removeCommandLineParser(this);
}

void PlaylistCommandLineParser::argsReceived(const QStringList & args, bool usingQtSingleApplication) {
	Q_UNUSED(usingQtSingleApplication);

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
		if (!playlistEnqueue) {
			_playlistModel->clear();
		}
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
