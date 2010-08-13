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

#ifndef LYRICSFETCHERTEST_H
#define LYRICSFETCHERTEST_H

#include <QtTest/QtTest>

#include <QtNetwork/QNetworkReply>

class ContentFetcherTrack;
class LyricsFetcher;

/**
 * Test for LyricsFetcher.
 *
 * @see LyricsFetcher
 * @author Tanguy Krotoff
 */
class LyricsFetcherTest : public QObject {
	Q_OBJECT
private slots:

	/** Called before the first testfunction is executed. */
	void initTestCase();

	/** Called after the last testfunction was executed. */
	void cleanupTestCase();

	void fetch_data();
	void fetch();

	void lyricsFound(QNetworkReply::NetworkError error, const QUrl & url, const QByteArray & lyrics, const ContentFetcherTrack & track);

private:

	LyricsFetcher * _lyricsFetcher;
};

#endif	//LYRICSFETCHERTEST_H
