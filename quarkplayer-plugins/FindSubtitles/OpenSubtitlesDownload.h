/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef OPENSUBTITLESDOWNLOAD_H
#define OPENSUBTITLESDOWNLOAD_H

#include <QtCore/QObject>

class QUrl;
class QNetworkAccessManager;
class QNetworkReply;

/**
 * Download a subtitle from http://www.opensubtitles.org website.
 *
 * Open Subtitles DataBase OSDb protocol: http://trac.opensubtitles.org/projects/opensubtitles/wiki/OSDb
 *
 * @author Tanguy Krotoff
 */
class OpenSubtitlesDownload : public QObject {
	Q_OBJECT
public:

	OpenSubtitlesDownload(QObject * parent);

	~OpenSubtitlesDownload();

	/**
	 * Downloads the subtitle given a file name.
	 *
	 * Will internally compute the hash of the video and
	 * send it to opensubtitles.org
	 *
	 * @param fileName video file name
	 * @return URL forged with the video file name or empty URL if something went wrong
	 */
	QUrl download(const QString & fileName);

	void download(const QUrl & url);

signals:

	void finished(QNetworkReply * reply);

	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:

	QNetworkAccessManager * _networkManager;

	QNetworkReply * _currentNetworkReply;
};

#endif	//OPENSUBTITLESDOWNLOAD_H
