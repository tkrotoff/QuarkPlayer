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

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtCore/QtConcurrentRun>
#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>

#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaType>
#include <QtCore/QDebug>

PlaylistParser::PlaylistParser(QObject * parent)
	: IPlaylistParser(parent) {

	qRegisterMetaType<QList<MediaInfo> >("QList<MediaInfo>");

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

void PlaylistParser::findParser(const QString & location) {
	QString extension(QFileInfo(location).suffix().toLower());

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
	}
}

void PlaylistParser::concurrentFinished() {
	_error = NoError;
	emit finished(_error, 0);
}

void PlaylistParser::stop() {
	if (_parser) {
		_parser->stop();
	}
}


PlaylistReader::PlaylistReader(QObject * parent)
	: PlaylistParser(parent) {

	_networkAccessManager = NULL;
}

PlaylistReader::~PlaylistReader() {
}

void PlaylistReader::load(const QString & location) {
	qDebug() << __FUNCTION__ << "location:" << location;

	if (MediaInfo::isUrl(location)) {
		if (!_networkAccessManager) {
			_networkAccessManager = new QNetworkAccessManager(this);
		}
		connect(_networkAccessManager, SIGNAL(finished(QNetworkReply *)),
			SLOT(networkReplyFinished(QNetworkReply *)));
		_networkAccessManager->get(QNetworkRequest(QUrl(location)));
	}

	else {
		//Means location is a local file

		QFile * file = new QFile(location, this);

		//Cannot use QIODevice::Text since binary playlist formats can exist (or exist already)
		//See QFile documentation, here a copy-paste:
		//The QIODevice::Text flag passed to open() tells Qt to convert Windows-style line terminators ("\r\n")
		//into C++-style terminators ("\n").
		//By default, QFile assumes binary, i.e. it doesn't perform any conversion on the bytes stored in the file.
		bool ok = file->open(QIODevice::ReadOnly);

		if (ok) {
			loadIODevice(file, location);
		} else {
			_error = FileNotFoundError;
			delete file;
		}
	}
}

void PlaylistReader::networkReplyFinished(QNetworkReply * reply) {
	QNetworkReply::NetworkError error = reply->error();
	if (error == QNetworkReply::NoError) {
		_error = NoError;
		loadIODevice(reply, reply->url().toString());
	} else {
		_error = NetworkError;
		emit finished(_error, 0);
	}
}

void PlaylistReader::loadIODevice(QIODevice * device, const QString & location) {
	if (_parser) {
		//Disconnect the previous parser if any
		disconnect(_parser);
	}

	findParser(location);
	if (_parser) {
		connect(_parser, SIGNAL(filesFound(const QList<MediaInfo> &)),
			SIGNAL(filesFound(const QList<MediaInfo> &)));

		static QFutureWatcher<void> * watcher = NULL;
		if (!watcher) {
			//Lazy initialization
			watcher = new QFutureWatcher<void>(this);
			connect(watcher, SIGNAL(finished()),
				SLOT(concurrentFinished()));
		}
		QFuture<void> future = QtConcurrent::run(_parser, &IPlaylistParserImpl::load, device, location);
		watcher->setFuture(future);
	}
}


PlaylistWriter::PlaylistWriter(QObject * parent)
	: PlaylistParser(parent) {
}

PlaylistWriter::~PlaylistWriter() {
}

void PlaylistWriter::save(const QString & location, const QList<MediaInfo> & files) {
	qDebug() << __FUNCTION__ << "location:" << location;

	if (MediaInfo::isUrl(location)) {
		qCritical() << __FUNCTION__ << "Writing to a remote location is not supported";
	}

	else {
		//Means location is a local file

		QFile * file = new QFile(location, this);

		//Cannot use QIODevice::Text since binary playlist formats can exist (or exist already)
		//See QFile documentation, here a copy-paste:
		//The QIODevice::Text flag passed to open() tells Qt to convert Windows-style line terminators ("\r\n")
		//into C++-style terminators ("\n").
		//By default, QFile assumes binary, i.e. it doesn't perform any conversion on the bytes stored in the file.
		bool ok = file->open(QIODevice::WriteOnly);

		if (ok) {
			saveIODevice(file, location, files);
		} else {
			_error = FileNotFoundError;
			delete file;
		}
	}
}

void PlaylistWriter::saveIODevice(QIODevice * device, const QString & location, const QList<MediaInfo> & files) {
	findParser(location);
	if (_parser) {
		static QFutureWatcher<void> * watcher = NULL;
		if (!watcher) {
			//Lazy initialization
			watcher = new QFutureWatcher<void>(this);
			connect(watcher, SIGNAL(finished()),
				SLOT(concurrentFinished()));
		}
		connect(watcher, SIGNAL(finished()),
			SLOT(concurrentFinished()));
		QFuture<void> future = QtConcurrent::run(_parser, &IPlaylistParserImpl::save, device, location, files);
		watcher->setFuture(future);
	}
}
