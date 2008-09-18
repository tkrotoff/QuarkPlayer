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

#include "PlaylistParser.h"

#include "M3UParser.h"

#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QDebug>

PlaylistParser::PlaylistParser(const QString & filename, QObject * parent)
	: IPlaylistParser(filename, parent) {

	_parser = NULL;

	_parserList += new M3UParser(filename, this);
	//_parserList += new PLSParser(filename, this);

	QString extension(QFileInfo(filename).suffix().toLower());

	foreach (IPlaylistParser * parser, _parserList) {
		if (parser->fileExtensions().contains(extension)) {
			//Found the right parser
			_parser = parser;
			connect(_parser, SIGNAL(filesFound(const QStringList &)),
				SIGNAL(filesFound(const QStringList &)));
			connect(_parser, SIGNAL(finished()),
				SIGNAL(finished()));
			break;
		}
	}

	if (!_parser) {
		qCritical() << __FUNCTION__ << "Error: no playlist parser available for this format:" << extension;
	}
}

PlaylistParser::~PlaylistParser() {
	//_loadFuture.waitForFinished();
	_saveFuture.waitForFinished();
}

QStringList PlaylistParser::fileExtensions() const {
	QStringList extensions;

	foreach (IPlaylistParser * parser, _parserList) {
		extensions << parser->fileExtensions();
	}

	return extensions;
}

void PlaylistParser::load() {
	if (_parser) {
		//_parser->load();
		_loadFuture = QtConcurrent::run(_parser, &IPlaylistParser::load);
	}
}

void PlaylistParser::save(const QStringList & files) {
	if (_parser) {
		//_parser->save(files);
		_saveFuture = QtConcurrent::run(_parser, &IPlaylistParser::save, files);
	}
}
