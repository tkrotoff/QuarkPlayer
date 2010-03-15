/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "OpenSubtitlesDownload.h"

#include "OpenSubtitlesParser.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtCore/QUrl>
#include <QtCore/QDebug>

OpenSubtitlesDownload::OpenSubtitlesDownload(QObject * parent)
	: QObject(parent) {

	_networkManager = new QNetworkAccessManager(this);
	connect(_networkManager, SIGNAL(finished(QNetworkReply *)),
		SIGNAL(finished(QNetworkReply *)));
	_currentNetworkReply = NULL;
}

OpenSubtitlesDownload::~OpenSubtitlesDownload() {
}

QUrl OpenSubtitlesDownload::download(const QString & fileName) {
	QUrl url;

	qDebug() << Q_FUNC_INFO << "Video file name:" << fileName;

	QString hash = OpenSubtitlesParser::calculateHash(fileName);
	if (hash.isEmpty()) {
		qCritical() << Q_FUNC_INFO << "Error: invalid empty hash";
	} else {
		url = QUrl("http://www.opensubtitles.org/en/search/sublanguageid-all/moviehash-" + hash + "/simplexml");
		download(url);
	}

	return url;
}

void OpenSubtitlesDownload::download(const QUrl & url) {
	qDebug() << Q_FUNC_INFO << "URL:" << url;

	if (_currentNetworkReply) {
		_currentNetworkReply->abort();
	}
	_currentNetworkReply = _networkManager->get(QNetworkRequest(url));
	connect(_currentNetworkReply, SIGNAL(downloadProgress(qint64, qint64)),
		SIGNAL(downloadProgress(qint64, qint64)));
}
