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
		SLOT(downloadFinished(QNetworkReply *)));
}

OpenSubtitlesDownload::~OpenSubtitlesDownload() {
}

void OpenSubtitlesDownload::download(const QString & fileName) {
	qDebug() << Q_FUNC_INFO << "Video file name:" << fileName;

	if (fileName.isEmpty()) {
		return;
	}

	QString hash = OpenSubtitlesParser::calculateHash(fileName);
	if (hash.isEmpty()) {
		qCritical() << Q_FUNC_INFO << "Error: invalid hash";
	} else {
		QString url = "http://www.opensubtitles.org/en/search/sublanguageid-all/moviehash-" + hash + "/simplexml";
		download(url);
	}
}

void OpenSubtitlesDownload::download(const QUrl & url) {
	qDebug() << Q_FUNC_INFO << "URL:" << url;

	QNetworkReply * reply = _networkManager->get(QNetworkRequest(url));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
		SLOT(downloadProgress(qint64, qint64)));
}

void OpenSubtitlesDownload::downloadFinished(QNetworkReply * reply) {
	QNetworkReply::NetworkError error = reply->error();
	QUrl url = reply->url();

	qDebug() << Q_FUNC_INFO << url << error << reply->errorString();

	switch (error) {
	case QNetworkReply::NoError: {
		QByteArray data(reply->readAll());

		if (url.toString().contains("simplexml")) {
			//This means we have finished downloading
			//the XML from opensubtitles.org
			emit finished(data);
			parseXml(data);
		} else {
			//This means we have finished downloading
			//an archive file from opensubtitles.org
			emit archiveDownloaded(data);
		}
		break;
	}

	default:
		break;
	}
}
