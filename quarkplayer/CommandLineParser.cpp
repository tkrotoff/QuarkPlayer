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

#include "CommandLineParser.h"

#include "CommandLineHelp.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <iostream>

#include <cstdio>

CommandLineParser::CommandLineParser() {
	run();
}

CommandLineParser::~CommandLineParser() {
}

QString CommandLineParser::fileToPlay() const {
	QString fileToPlay;

	if (!_filesForPlaylist.isEmpty()) {
		fileToPlay = _filesForPlaylist[0];
	}

	return fileToPlay;
}

QStringList CommandLineParser::filesForPlaylist() const {
	return _filesForPlaylist;
}

void CommandLineParser::run() {
	QStringList args(QCoreApplication::arguments());

	//Delete the first one in the list,
	//it is the name of the application
	//we don't care about this
	args.removeAt(0);

	foreach (QString arg, args) {

		if ((arg == "--help") || (arg == "-help") || (arg == "-h") || (arg == "-?")) {
			CommandLineHelp help;
			std::cout << help.toString().toUtf8().constData();
			exit(EXIT_SUCCESS);
		}

		else {
			qDebug() << __FUNCTION__ << "File to play:" << arg;

			_filesForPlaylist << arg;
		}
	}
}
