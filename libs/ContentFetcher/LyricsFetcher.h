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

#ifndef LYRICSFETCHER_H
#define LYRICSFETCHER_H

#include <ContentFetcher/ContentFetcher.h>

#include <QtCore/QUrl>

class QNetworkAccessManager;

/**
 * Gets the lyric of a song from LyricWiki.org.
 *
 * @see http://lyricwiki.org
 * @author Tanguy Krotoff
 */
class CONTENTFETCHER_API LyricsFetcher : public ContentFetcher {
	Q_OBJECT
public:

	LyricsFetcher(QObject * parent);

	~LyricsFetcher();

	void start(const ContentFetcherTrack & track, const QString & language = QString());

private slots:

	void gotLyricsUrl(QNetworkReply * reply);

	void gotLyrics(QNetworkReply * reply);

private:

	QUrl lyricWikiUrl(const QString & artist, const QString & title) const;

	QNetworkAccessManager * _lyricsDownloader;
};

#endif	//LYRICSFETCHER_H
