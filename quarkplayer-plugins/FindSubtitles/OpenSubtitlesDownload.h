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

#ifndef OPENSUBTITLESDOWNLOAD_H
#define OPENSUBTITLESDOWNLOAD_H

#include <QtCore/QObject>

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

public slots:

	void download(const QString & fileName);

signals:

	void subtitleDownloaded(const QString & fileName);

private slots:

	void downloadFinished(QNetworkReply * reply);

private:

	void download(const QUrl & url);
};

#endif	//OPENSUBTITLESDOWNLOAD_H
