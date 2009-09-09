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

#include "WikipediaArticleTest.h"

#include <contentfetcher/WikipediaArticle.h>

void WikipediaArticleTest::initTestCase() {
	_wikipediaArticle = new WikipediaArticle(this);
	connect(_wikipediaArticle,
		SIGNAL(contentFound(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &)),
		SLOT(wikipediaArticleFound(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &))
	);
}

void WikipediaArticleTest::cleanupTestCase() {
	delete _wikipediaArticle;
}

void WikipediaArticleTest::fetch_data() {
	QTest::addColumn<QString>("artist");
	QTest::addColumn<int>("networkError");
	QTest::addColumn<QString>("wikipediaArticleUrl");
	QTest::addColumn<QString>("wikipediaArticleContent");

	QTest::newRow("Michael Jackson") << "Michael Jackson"
		<< static_cast<int>(QNetworkReply::NoError)
		<< "http://en.wikipedia.org/wiki/Michael_Jackson"
		<< "<html><body><div id=\"bodyContent\".*</div></body></html>";

	QTest::newRow("Muse") << "Muse"
		<< static_cast<int>(QNetworkReply::NoError)
		<< "http://en.wikipedia.org/wiki/Muse_(band)"
		<< "<html><body><div id=\"bodyContent\".*</div></body></html>";

	QTest::newRow("Noir Désir") << "Noir Désir"
		<< static_cast<int>(QNetworkReply::NoError)
		<< "http://en.wikipedia.org/wiki/Noir_D%C3%A9sir"
		<< "";

	QTest::newRow("Non existing") << "Non existing artist"
		<< static_cast<int>(QNetworkReply::ContentNotFoundError)
		<< "http://en.wikipedia.org/w/api.php?action=opensearch&search=Non existing artist&format=xml"
		<< "";

	QTest::newRow("Empty") << ""
		<< static_cast<int>(QNetworkReply::ContentNotFoundError)
		<< "http://en.wikipedia.org/w/api.php?action=opensearch&search=&format=xml"
		<< "<html><body><div id=\"bodyContent\"</div></body></html>";
}

void WikipediaArticleTest::fetch() {
	QFETCH(QString, artist);

	//
	ContentFetcherTrack track;
	track.artist = artist;
	_wikipediaArticle->start(track, "en");
	QTestEventLoop::instance().enterLoop(30);
	QVERIFY(!QTestEventLoop::instance().timeout());
	///
}

void WikipediaArticleTest::wikipediaArticleFound(QNetworkReply::NetworkError error, const QUrl & url, const QByteArray & wikipediaArticle, const ContentFetcherTrack & track) {
	QString tmp = url.toString();

	QFETCH(QString, artist);
	QFETCH(int, networkError);
	QFETCH(QString, wikipediaArticleUrl);
	QFETCH(QString, wikipediaArticleContent);

	QCOMPARE(artist, track.artist);
	QCOMPARE(networkError, static_cast<int>(error));
	QCOMPARE(wikipediaArticleUrl, tmp);
	//QCOMPARE(wikipediaArticleContent, QString::fromUtf8(wikipediaArticle));

	QTestEventLoop::instance().exitLoop();
}
