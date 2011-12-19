/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "LyricsFetcherTest.h"

#include <ContentFetcher/LyricsFetcher.h>

QTEST_MAIN(LyricsFetcherTest)

void disableMessageOutput(QtMsgType type, const char * msg) {
	Q_UNUSED(type);
	Q_UNUSED(msg);
}

void LyricsFetcherTest::initTestCase() {
	//Disable qDebug() and friends
	qInstallMsgHandler(disableMessageOutput);
	///

	_lyricsFetcher = new LyricsFetcher(this);
	connect(_lyricsFetcher,
		SIGNAL(finished(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &)),
		SLOT(lyricsFound(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &))
	);
}

void LyricsFetcherTest::cleanupTestCase() {
	delete _lyricsFetcher;
}

void LyricsFetcherTest::fetch_data() {
	QTest::addColumn<QString>("artist");
	QTest::addColumn<QString>("title");

	QTest::addColumn<int>("lyricsError");
	QTest::addColumn<QString>("lyricsUrl");
	QTest::addColumn<QRegExp>("lyricsContent");

	QTest::newRow("Michael Jackson") << "Michael Jackson" << "Don't Stop 'Til You Get Enough"
		<< static_cast<int>(QNetworkReply::NoError)
		<< "http://lyrics.wikia.com/Michael_Jackson:Don't_Stop_'Til_You_Get_Enough"
		<< QRegExp("^&#40;&#115;&#112;&#111;&#107;&#101;&#110;&#41;<br />&#121;&#111;&#117;&#32;&#107;&#110;&#111;&#119;&#44;&#32;&#73;&#32;&#119;&#97;&#115;&#44;&#32;&#73;&#32;&#119;&#97;&#115;&#32;&#119;&#111;&#110;&#100;&#101;&#114;&#105;&#110;&#103;&#32;&#105;&#102;&#44;&#32;<br />&#105;&#102;&#32;&#121;&#111;&#117;&#32;&#99;&#111;&#117;&#108;&#100;&#32;&#107;&#101;&#101;&#112;&#32;&#111;&#110;&#44;<br />&#98;&#101;&#99;&#97;&#117;&#115;&#101;&#32;&#116;&#104;&#101;&#32;&#102;&#111;&#114;&#99;&#101;&#44;&#32;<br />&#105;&#.*$");

	QTest::newRow("Noir Désir") << "Noir Désir" << "L'Homme Pressé"
		<< static_cast<int>(QNetworkReply::NoError)
		<< "http://lyrics.wikia.com/Noir_Désir:L'homme_Pressé"
		<< QRegExp("");

	QTest::newRow("Non existing") << "Non existing artist" << "Non existing title"
		<< static_cast<int>(QNetworkReply::ContentNotFoundError)
		<< "http://lyrics.wikia.com/Main_Page"
		<< QRegExp("");

	QTest::newRow("Empty") << "" << ""
		<< static_cast<int>(QNetworkReply::ContentNotFoundError)
		<< "http://lyrics.wikia.com/Main_Page"
		<< QRegExp("");
}

void LyricsFetcherTest::fetch() {
	QSignalSpy spyFinished(_lyricsFetcher, SIGNAL(finished(QNetworkReply::NetworkError,
				const QUrl &, const QByteArray &, const ContentFetcherTrack &)));

	QFETCH(QString, artist);
	QFETCH(QString, title);

	ContentFetcherTrack track;
	track.artist = artist;
	track.title = title;
	_lyricsFetcher->start(track);
	QTestEventLoop::instance().enterLoop(30);
	QVERIFY(!QTestEventLoop::instance().timeout());

	QCOMPARE(spyFinished.count(), 1);
}

void LyricsFetcherTest::lyricsFound(QNetworkReply::NetworkError error, const QUrl & url, const QByteArray & lyrics, const ContentFetcherTrack & track) {
	QFETCH(QString, artist);
	QCOMPARE(artist, track.artist);

	QFETCH(QString, title);
	QCOMPARE(title, track.title);

	QFETCH(int, lyricsError);
	QCOMPARE(lyricsError, static_cast<int>(error));

	QFETCH(QString, lyricsUrl);
	QCOMPARE(lyricsUrl, url.toString());

	QFETCH(QRegExp, lyricsContent);
	QVERIFY(lyricsContent.exactMatch(QString::fromUtf8(lyrics)));

	QTestEventLoop::instance().exitLoop();
}
