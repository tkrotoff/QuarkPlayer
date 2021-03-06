/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QtNetwork/QNetworkReply>

#include <QtCore/QObject>
#include <QtCore/QEventLoop>

class QNetworkAccessManager;
class QUrl;

/**
 * Simple class that downloads an URL and save it to a file.
 *
 * The download is done in a synchronous way.
 *
 * Example:
 * <pre>
 * Download::download(NULL, "http://download.blender.org/peach/trailer/trailer_iphone.m4v");
 * QString fileName = QCoreApplication::applicationDirPath() + QDir::separator() + "trailer_iphone.m4v";
 * </pre>
 *
 * @author Tanguy Krotoff
 * @see QNetworkAccessManager
 */
class Download : public QObject {
	Q_OBJECT
public:

	/**
	 * Example:
	 * http://download.blender.org/peach/trailer/trailer_iphone.m4v
	 */
	static QNetworkReply::NetworkError download(QObject * parent, const QUrl & url, const QString & fileName = QString());

private slots:

	void finished(QNetworkReply * reply);

private:

	Download(const QString & fileName, QObject * parent);

	~Download();

	QString _fileName;

	QNetworkAccessManager * _networkManager;

	QNetworkReply::NetworkError _error;

	/** Event loop used to block until HTTP request finished. */
	QEventLoop _loop;
};

#endif	//DOWNLOAD_H
