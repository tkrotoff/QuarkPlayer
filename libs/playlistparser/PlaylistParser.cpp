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

#include "PlaylistParser.h"

#include "PlaylistParserThread.h"

#include "M3UParser.h"
#include "WPLParser.h"
#include "PLSParser.h"
#include "ASXParser.h"
#include "XSPFParser.h"

#include <mediainfowindow/MediaInfo.h>

#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaType>
#include <QtCore/QDebug>

PlaylistParser::PlaylistParser(const QString & filename, QObject * parent)
	: IPlaylistParser(filename, parent) {

	qRegisterMetaType<QList<MediaInfo> >("QList<MediaInfo>");

	_parser = NULL;
	_parserThread = NULL;

	//When to delete _parserList ?
	_parserList += new M3UParser(filename, this);
	_parserList += new PLSParser(filename, this);
	_parserList += new WPLParser(filename, this);
	_parserList += new ASXParser(filename, this);
	_parserList += new XSPFParser(filename, this);

	QString extension(QFileInfo(filename).suffix().toLower());

	foreach (IPlaylistParser * parser, _parserList) {
		if (parser->fileExtensions().contains(extension)) {
			//Found the right parser
			_parser = parser;
			_parserThread = new PlaylistParserThread(_parser);

			connect(_parser, SIGNAL(filesFound(const QList<MediaInfo> &)),
				SIGNAL(filesFound(const QList<MediaInfo> &)));
			connect(_parser, SIGNAL(finished(int)),
				SIGNAL(finished(int)));
			break;
		}
	}

	if (!_parser) {
		qCritical() << __FUNCTION__ << "Error: no playlist parser available for this format:" << extension;
	}
}

PlaylistParser::~PlaylistParser() {
	stop();
}

QStringList PlaylistParser::fileExtensions() const {
	QStringList extensions;

	foreach (IPlaylistParser * parser, _parserList) {
		extensions << parser->fileExtensions();
	}

	return extensions;
}

void PlaylistParser::load() {
	if (_parserThread) {
		_parserThread->load();
	}
}

void PlaylistParser::save(const QList<MediaInfo> & files) {
	if (_parserThread) {
		_parserThread->save(files);
	}
}

void PlaylistParser::stop() {
	if (_parserThread) {
		_parserThread->stop();
		_parserThread->wait();
	}
}
