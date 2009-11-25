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

#include "M3UParser.h"
#include "WPLParser.h"
#include "PLSParser.h"
#include "ASXParser.h"
#include "XSPFParser.h"
#include "CUEParser.h"

#include <mediainfofetcher/MediaInfo.h>

#include <QtCore/QtConcurrentRun>
#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>

#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaType>
#include <QtCore/QDebug>

PlaylistParser::PlaylistParser(QObject * parent)
	: IPlaylistParser(parent),
	_file(this) {

	qRegisterMetaType<QList<MediaInfo> >("QList<MediaInfo>");
	qRegisterMetaType<PlaylistParser::Error>("PlaylistParser::Error");

	_parser = NULL;
	_error = NoError;

	//FIXME memory leak: when to delete _parserList ?
	_parserList += new M3UParser(this);
	_parserList += new PLSParser(this);
	_parserList += new WPLParser(this);
	_parserList += new ASXParser(this);
	_parserList += new XSPFParser(this);
	_parserList += new CUEParser(this);
}

PlaylistParser::~PlaylistParser() {
	//FIXME memory leak: when to delete _parserList ?
	foreach (IPlaylistParserImpl * parser, _parserList) {
		parser->stop();
		//This is dangerous
		//delete parser;
	}
}

const QFile & PlaylistParser::file() const {
	return _file;
}

void PlaylistParser::findParser(const QString & fileName) {
	QString extension(QFileInfo(fileName).suffix().toLower());

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
	_error = NoError;
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
	qDebug() << __FUNCTION__ << "fileName:" << fileName;

	_timeElapsed.start();

	_file.setFileName(fileName);

	//Cannot use QIODevice::Text since binary playlist formats can exist (or exist already)
	//See QFile documentation, here a copy-paste:
	//The QIODevice::Text flag passed to open() tells Qt to convert Windows-style line terminators ("\r\n")
	//into C++-style terminators ("\n").
	//By default, QFile assumes binary, i.e. it doesn't perform any conversion on the bytes stored in the file.
	bool ok = _file.open(QIODevice::ReadOnly);

	if (ok) {
		loadIODevice(&_file, fileName);
	} else {
		_error = FileError;
		emit finished(_error, _timeElapsed.elapsed());
	}
}

void PlaylistReader::loadIODevice(QIODevice * device, const QString & fileName) {
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

		static QFutureWatcher<void> * watcher = NULL;
		if (!watcher) {
			//Lazy initialization
			watcher = new QFutureWatcher<void>(this);
			connect(watcher, SIGNAL(finished()),
				SLOT(concurrentFinished()));
		}
		QFuture<void> future = QtConcurrent::run(_parser, &IPlaylistParserImpl::load, device, fileName);
		watcher->setFuture(future);
	}
}


PlaylistWriter::PlaylistWriter(QObject * parent)
	: PlaylistParser(parent) {
}

PlaylistWriter::~PlaylistWriter() {
}

void PlaylistWriter::save(const QString & fileName, const QList<MediaInfo> & files) {
	qDebug() << __FUNCTION__ << "fileName:" << fileName;

	_timeElapsed.restart();

	_file.setFileName(fileName);

	//Cannot use QIODevice::Text since binary playlist formats may exist (or exist already)
	//See QFile documentation, here a copy-paste:
	//The QIODevice::Text flag passed to open() tells Qt to convert Windows-style line terminators ("\r\n")
	//into C++-style terminators ("\n").
	//By default, QFile assumes binary, i.e. it doesn't perform any conversion on the bytes stored in the file.
	bool ok = _file.open(QIODevice::WriteOnly);

	if (ok) {
		saveIODevice(&_file, fileName, files);
	} else {
		_error = FileError;
		emit finished(_error, _timeElapsed.elapsed());
	}
}

void PlaylistWriter::saveIODevice(QIODevice * device, const QString & fileName, const QList<MediaInfo> & files) {
	findParser(fileName);
	if (_parser) {
		static QFutureWatcher<void> * watcher = NULL;
		if (!watcher) {
			//Lazy initialization
			watcher = new QFutureWatcher<void>(this);
			connect(watcher, SIGNAL(finished()),
				SLOT(concurrentFinished()));
		}
		QFuture<void> future = QtConcurrent::run(_parser, &IPlaylistParserImpl::save, device, fileName, files);
		watcher->setFuture(future);
	}
}
