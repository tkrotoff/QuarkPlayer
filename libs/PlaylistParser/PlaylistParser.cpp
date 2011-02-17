/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PlaylistParser.h"

#include "M3UParser.h"
#include "WPLParser.h"
#include "PLSParser.h"
#include "ASXParser.h"
#include "XSPFParser.h"
#include "CUEParser.h"
#include "PlaylistParserLogger.h"

#include <MediaInfoFetcher/MediaInfo.h>

#include <QtCore/QtConcurrentRun>
#include <QtCore/QFuture>

#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaType>

PlaylistParser::PlaylistParser(QObject * parent)
	: IPlaylistParser(parent) {

	qRegisterMetaType<QList<MediaInfo> >("QList<MediaInfo>");
	qRegisterMetaType<PlaylistParser::Error>("PlaylistParser::Error");

	_parser = NULL;
	_error = NoError;

	_watcher = new QFutureWatcher<bool>(this);
	connect(_watcher, SIGNAL(finished()),
		SLOT(concurrentFinished()));

	//FIXME memory leak: when to delete _parserList?
	//Use shared pointer?
	_parserList += new M3UParser(this);
	_parserList += new PLSParser(this);
	_parserList += new WPLParser(this);
	_parserList += new ASXParser(this);
	_parserList += new XSPFParser(this);
	_parserList += new CUEParser(this);
}

PlaylistParser::~PlaylistParser() {
	//FIXME memory leak: when to delete _parserList?
	//Use shared pointer?
	foreach (IPlaylistParserImpl * parser, _parserList) {
		parser->stop();
		//This is dangerous
		//delete parser;
	}
}

//FIXME Change the API and make it return the parser?
void PlaylistParser::findParser(const QString & fileName) {
	QString extension(QFileInfo(fileName).suffix());

	_parser = NULL;
	foreach (IPlaylistParserImpl * parser, _parserList) {
		if (parser->fileExtensions().contains(extension, Qt::CaseInsensitive)) {
			//Found the right parser
			_parser = parser;
			break;
		}
	}

	if (!_parser) {
		_error = UnsupportedFormatError;
		emit finished(_error, _timeElapsed.elapsed());
	}
}

void PlaylistParser::concurrentFinished() {
	QFuture<bool> future = _watcher->future();
	bool ok = future.result();
	if (ok) {
		_error = NoError;
	} else {
		_error = IOError;
	}
	emit finished(_error, _timeElapsed.elapsed());
}

void PlaylistParser::stop() {
	if (_parser) {
		_parser->stop();
	}
}


PlaylistReader::PlaylistReader(QObject * parent)
	: PlaylistParser(parent) {
}

PlaylistReader::~PlaylistReader() {
}

void PlaylistReader::load(const QString & fileName) {
	PlaylistParserDebug() << "fileName:" << fileName;

	_timeElapsed.start();

	if (_parser) {
		//Disconnect the previous parser if any
		disconnect(_parser);
	}

	findParser(fileName);
	if (_parser) {
		disconnect(_parser, SIGNAL(filesFound(const QList<MediaInfo> &)),
			this, SIGNAL(filesFound(const QList<MediaInfo> &)));
		connect(_parser, SIGNAL(filesFound(const QList<MediaInfo> &)),
			SIGNAL(filesFound(const QList<MediaInfo> &)));

		QFuture<bool> future = QtConcurrent::run(
			_parser, &IPlaylistParserImpl::load, fileName
		);
		_watcher->setFuture(future);
	}
}


PlaylistWriter::PlaylistWriter(QObject * parent)
	: PlaylistParser(parent) {
}

PlaylistWriter::~PlaylistWriter() {
}

void PlaylistWriter::save(const QString & fileName, const QList<MediaInfo> & mediaList) {
	PlaylistParserDebug() << "fileName:" << fileName;

	_timeElapsed.restart();

	findParser(fileName);
	if (_parser) {
		QFuture<bool> future = QtConcurrent::run(
			_parser, &IPlaylistParserImpl::save, fileName, mediaList
		);
		_watcher->setFuture(future);
	}
}
