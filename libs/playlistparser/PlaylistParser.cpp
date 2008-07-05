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
#include <QtCore/QDebug>

PlaylistParser::PlaylistParser(const QString & filename)
	: IPlaylistParser(filename) {

	_parser = NULL;

	_parserList += new M3UParser(filename);
	//_parserList += new PLSParser(filename);

	QString extension(QFileInfo(filename).suffix().toLower());

	foreach (IPlaylistParser * parser, _parserList) {
		if (parser->fileExtensions().contains(extension)) {
			_parser = parser;
		}
	}

	if (!_parser) {
		qCritical() << __FUNCTION__ << "Error: no playlist parser available for this format:" << extension;
	}
}

PlaylistParser::~PlaylistParser() {
	_parserList.clear();
}

QStringList PlaylistParser::fileExtensions() const {
	QStringList extensions;

	foreach (IPlaylistParser * parser, _parserList) {
		extensions << parser->fileExtensions();
	}

	return extensions;
}

QStringList PlaylistParser::load() {
	QStringList files;
	if (_parser) {
		files = _parser->load();
	}
	return files;
}

bool PlaylistParser::save(const QStringList & files) {
	bool ret = false;
	if (_parser) {
		ret = _parser->save(files);
	}
	return ret;
}
