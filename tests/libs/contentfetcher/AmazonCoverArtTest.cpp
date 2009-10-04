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

#include "AmazonCoverArtTest.h"

#include <contentfetcher/AmazonCoverArt.h>

#include <QtCore/QRegExp>

//Please don't copy this to another program; keys are free from aws.amazon.com
//Your Access Key ID:
static const char * AMAZON_WEB_SERVICE_ACCESS_KEY_ID = "1BPZGMNT4PWSJS6NHG02";
//Your Secret Access Key:
static const char * AMAZON_WEB_SERVICE_SECRET_KEY = "RfD3RoKwZ+5GpJa/i03jhoiDZM26OAc+TPpXMps0";

void AmazonCoverArtTest::initTestCase() {
	_amazonCoverArt = new AmazonCoverArt(AMAZON_WEB_SERVICE_ACCESS_KEY_ID, AMAZON_WEB_SERVICE_SECRET_KEY, this);
	connect(_amazonCoverArt,
		SIGNAL(finished(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &)),
		SLOT(amazonCoverArtFound(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, const ContentFetcherTrack &))
	);
}

void AmazonCoverArtTest::cleanupTestCase() {
	delete _amazonCoverArt;
}

void AmazonCoverArtTest::fetch_data() {
	QTest::addColumn<QString>("artist");
	QTest::addColumn<QString>("album");
	QTest::addColumn<QString>("amazonASIN");

	QTest::addColumn<int>("networkError");
	QTest::addColumn<QRegExp>("amazonCoverArtUrl");
	QTest::addColumn<int>("amazonCoverArtContentSize");

	QTest::newRow("Michael Jackson") << "Michael Jackson" << "Off the Wall" << ""
		<< static_cast<int>(QNetworkReply::NoError)
		<< QRegExp("http://ecx.images-amazon.com/images/I/51mV9uUEKcL.jpg")
		<< 44481;

	QTest::newRow("Noir Désir") << "Noir Désir" << "666.667 Club" << ""
		<< static_cast<int>(QNetworkReply::NoError)
		<< QRegExp("http://ecx.images-amazon.com/images/I/51me4zSIBML.jpg")
		<< 61027;

	QTest::newRow("Non existing") << "Non existing artist" << "Non existing album" << ""
		<< static_cast<int>(QNetworkReply::ContentNotFoundError)
		<< QRegExp("http://webservices.amazon.com/onca/xml.*")
		<< 0;

	QTest::newRow("Empty") << "" << "" << ""
		<< static_cast<int>(QNetworkReply::ContentNotFoundError)
		<< QRegExp("http://webservices.amazon.com/onca/xml.*")
		<< 0;

	QTest::newRow("ASIN") << "" << "" << "B00005QGAT"
		<< static_cast<int>(QNetworkReply::NoError)
		<< QRegExp("http://ecx.images-amazon.com/images/I/51mV9uUEKcL.jpg")
		<< 44481;
}

void AmazonCoverArtTest::fetch() {
	QFETCH(QString, artist);
	QFETCH(QString, album);
	QFETCH(QString, amazonASIN);

	//
	ContentFetcherTrack track;
	track.artist = artist;
	track.album = album;
	track.amazonASIN = amazonASIN;
	_amazonCoverArt->start(track);
	QTestEventLoop::instance().enterLoop(30);
	QVERIFY(!QTestEventLoop::instance().timeout());
	///
}

void AmazonCoverArtTest::amazonCoverArtFound(QNetworkReply::NetworkError error, const QUrl & url, const QByteArray & amazonCoverArt, const ContentFetcherTrack & track) {
	QFETCH(QString, artist);
	QFETCH(QString, album);
	QFETCH(QString, amazonASIN);
	QFETCH(int, networkError);
	QFETCH(QRegExp, amazonCoverArtUrl);
	QFETCH(int, amazonCoverArtContentSize);

	QCOMPARE(artist, track.artist);
	QCOMPARE(album, track.album);
	QCOMPARE(amazonASIN, track.amazonASIN);
	QCOMPARE(networkError, static_cast<int>(error));
	QVERIFY(amazonCoverArtUrl.exactMatch(url.toString()));

	QCOMPARE(amazonCoverArtContentSize, amazonCoverArt.size());

	QTestEventLoop::instance().exitLoop();
}
