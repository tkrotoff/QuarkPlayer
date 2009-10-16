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

#include "Download.h"

#include <QtNetwork/QNetworkAccessManager>

#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

Download::Download(const QString & fileName, QObject * parent)
	: QObject(parent) {

	_fileName = fileName;

	_networkManager = new QNetworkAccessManager(this);
	connect(_networkManager, SIGNAL(finished(QNetworkReply *)),
		SLOT(finished(QNetworkReply *)));

	_error = QNetworkReply::NoError;
}

Download::~Download() {
}

QNetworkReply::NetworkError Download::download(QObject * parent, const QUrl & url, const QString & fileName) {
	Download download(fileName, parent);
	download._networkManager->get(QNetworkRequest(url));

	//Block until the request is finished
	download._loop.exec();

	return download._error;
}

void Download::finished(QNetworkReply * reply) {
	_error = reply->error();

	if (_error == QNetworkReply::NoError) {
		QUrl url = reply->url();
		QString path = url.path();
		if (_fileName.isEmpty()) {
			_fileName = QFileInfo(path).fileName();
			_fileName = QCoreApplication::applicationDirPath() + QDir::separator() + _fileName;
		}

		QFile file(_fileName);
		if (file.open(QIODevice::WriteOnly)) {
			file.write(reply->readAll());
			file.close();
		} else {
			_error = QNetworkReply::UnknownContentError;
		}

	}

	//End the loop
	_loop.exit();
}
