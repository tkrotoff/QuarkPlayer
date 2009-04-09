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

#include "CommandLineHelp.h"

#include <filetypes/FileTypes.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>

#include <iostream>

CommandLineParser::CommandLineParser() {
	start();
}

CommandLineParser::~CommandLineParser() {
}

QString CommandLineParser::fileToPlay() const {
	QString fileToPlay;

	if (!_filesForPlaylist.isEmpty()) {
		QString filename(_filesForPlaylist[0]);
		bool isMultimediaFile = FileTypes::extensions(FileType::Video, FileType::Audio).contains(QFileInfo(filename).suffix(), Qt::CaseInsensitive);
		if (isMultimediaFile) {
			//The file is a playable file
			fileToPlay = filename;
		}
	}

	return fileToPlay;
}

QStringList CommandLineParser::filesForPlaylist() const {
	return _filesForPlaylist;
}

void CommandLineParser::start() {
	QStringList args(QCoreApplication::arguments());

	//Delete the first one in the list,
	//it is the name of the application
	//we don't care about this
	args.removeAt(0);

	foreach (QString arg, args) {

		if ((arg == "--help") || (arg == "-help") || (arg == "-h") || (arg == "-?")) {
			CommandLineHelp help;
			std::cout << help.toString().toUtf8().constData();

			//Quits the application
			QCoreApplication::quit();
		}

		else {
			qDebug() << __FUNCTION__ << "File to play:" << arg;

			_filesForPlaylist << arg;
		}
	}
}
