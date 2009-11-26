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

#include "PlaylistParserTest.h"

#include <PlaylistParser/PlaylistParser.h>

#include <MediaInfoFetcher/MediaInfo.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

QTEST_MAIN(PlaylistParserTest)

void PlaylistParserTest::initTestCase() {
	_parser = new PlaylistReader(this);
	connect(_parser, SIGNAL(filesFound(const QList<MediaInfo> &)),
		SLOT(filesFound(const QList<MediaInfo> &)));
	connect(_parser, SIGNAL(finished(PlaylistParser::Error, int)),
		SLOT(finished(PlaylistParser::Error, int)));
}

void PlaylistParserTest::cleanupTestCase() {
	//Wait for the user to press a key
	getchar();
}

void PlaylistParserTest::init() {
}

void PlaylistParserTest::cleanup() {
}

void PlaylistParserTest::loadPlaylist_data() {
	QTest::addColumn<QString>("playlistFileName");

	QTest::addColumn<QStringList>("filesFound");
	QTest::addColumn<int>("parserError");
	QTest::addColumn<int>("parserFileError");
	QTest::addColumn<int>("parserTimeElapsed");

	QTest::newRow("M3U") << ":/foobar2000-0.9.6.7/test.m3u"
		<< (QStringList()
			<< "01-Do Whats Good For Me.mp3"
			<< "02-No Limit.mp3"
			<< "03-Get Ready For This.mp3"
			<< "04-Twilight Zone.mp3"
			<< "05-No One.mp3"
			<< "06-Jump For Joy.mp3"
			<< "07-Tribal Dance.mp3"
			<< "08-Magic Friend.mp3"
			<< "09-Workaholic.mp3"
			<< "10-Let The Beat Control Your Body.mp3"
			<< "11-Nothing Like The Rain.mp3"
			<< "12-Spread Your Love.mp3"
			<< "13-The Real Thing.mp3"
			<< "14-Here I Go.mp3"
			<< "15-Maximum Overdrive.mp3"
			<< "16-Faces.mp3"
			<< "01 Blondie - Heart Of Glass.mp3"
			<< "02 Blondie - Dreaming.mp3"
			<< "03 Blondie - The Tide Is High.mp3"
			<< "04 Blondie - In The Flesh.mp3"
			<< "05 Blondie - Sunday Girl.mp3"
			<< "06 Blondie - Hanging On The Telephone.mp3"
			<< "07 Blondie - Rapture.mp3"
			<< "08 Blondie - One Way Or Another.mp3"
			<< "09 Blondie - (I'm Always Touched By Your) Presence Dear.mp3"
			<< "10 Blondie - Call Me (Theme From American Gigolo).mp3"
			<< "11 Blondie - Atomic.mp3"
			<< "12 Blondie - Rip Her To Shreds.mp3"
			<< "01  Madcon - Beggin'.mp3"
			<< "02  Madcon - Back On The Road [With Paperboys].mp3"
			<< "03  Madcon - Liar.mp3"
			<< "04  Madcon - Hard To Read [With Noora].mp3"
			<< "05  Madcon - Life's Too Short.mp3"
			<< "06  Madcon - The Way We Do Thangs [With Timbuktu].mp3"
			<< "07  Madcon - Blessed.mp3"
			<< "08  Madcon - Suda Suda [With El Axel].mp3"
			<< "09  Madcon - Let It Be Known.mp3"
			<< "10  Madcon - Let's Dance Instead.mp3"
			<< "11  Madcon - Dandelion.mp3"
			<< "12  Madcon - Pride And Prejudice [With Sofian].mp3"
			<< "13  Madcon - Me And My Brother.mp3"
			<< "14  Madcon - Loose [Bonus].mp3")
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//Non existing file
	QTest::newRow("Non existing File") << "Non existing File"
		<< QStringList()
		<< static_cast<int>(PlaylistParser::FileError)
		<< static_cast<int>(QFile::OpenError)
		<< 0;

	//Unsupported Format
	QTest::newRow("Unsupported Format") << ":/TheKMPlayer-2.9.4/test.KPL"
		<< QStringList()
		<< static_cast<int>(PlaylistParser::UnsupportedFormatError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//Europe1
	QTest::newRow("Europe1 M3U") << ":/fromInternet/webradio/europe1/europe1.m3u"
		<< (QStringList()
			<< "http://vipicecast.yacast.net/europe1")
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	QTest::newRow("Europe1 ASX") << ":/fromInternet/webradio/europe1/europe1.asx"
		<< (QStringList()
			<< "mms://viplagardere.yacast.net/encodereurope1")
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;
	///

	//France Inter
	QTest::newRow("France Inter M3U") << ":/fromInternet/webradio/franceinter/france_inter_mp3-128k.m3u"
		<< (QStringList()
			<< "http://mp3.live.tv-radio.com/franceinter/all/franceinterhautdebit.mp3")
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	QTest::newRow("France Inter PLS") << ":/fromInternet/webradio/franceinter/france_inter_mp3-128k.pls"
		<< (QStringList()
			<< "http://mp3.live.tv-radio.com/franceinter/all/franceinterhautdebit.mp3"
			<< "http://mp3.live.tv-radio.com/franceinter/all/franceinterhautdebit.mp3")
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;
	///

	//Radio Nova
	QTest::newRow("Radio Nova M3U") << ":/fromInternet/webradio/radionova/radionova.m3u"
		<< (QStringList()
			<< "http://broadcast.infomaniak.net:80/radionova-high.mp3")
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;
	///

	//Live9
	QTest::newRow("Live9 M3U") << ":/fromInternet/webradio/live9/192.m3u"
		<< (QStringList()
			<< "http://stream192.live9.fr:8050"
			<< "http://stream192.live9.fr:8050"
			<< "http://stream192.live9.fr:8050"
			<< "http://stream192.live9.fr:8050"
			<< "http://stream192.live9.fr:8050")
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	QTest::newRow("Live9 ASX") << ":/fromInternet/webradio/live9/192.asx"
		<< (QStringList()
			<< "mms://wm.live9.fr/live9live")
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;
	///

	//OUI FM
	QTest::newRow("OUI FM ASX") << ":/fromInternet/webradio/ouifm/Ouifm-hautdebit-wmp.asx"
		<< (QStringList()
			<< "http://www.ouifm.fr/player/IntroPubOUIFM.mp3"
			<< "http://broadcast.infomaniak.net:80/ouifm-high.mp3")
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;
	///

	//Virgin Radio
	QTest::newRow("Virgin Radio ASX") << ":/fromInternet/webradio/virginradio/virgin.asx"
		<< (QStringList()
			<< "mms://viplagardere.yacast.net/encodereurope2")
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;
	///
}

void PlaylistParserTest::loadPlaylist() {
	QSignalSpy spyFilesFound(_parser, SIGNAL(filesFound(const QList<MediaInfo> &)));
	QSignalSpy spyFinished(_parser, SIGNAL(finished(PlaylistParser::Error, int)));

	QFETCH(QString, playlistFileName);
	_parser->load(playlistFileName);

	//finished() signal might be already sent by load() method
	//if the file couldn't be opened
	if (spyFinished.count() == 0) {
		QTestEventLoop::instance().enterLoop(30);
		QVERIFY(!QTestEventLoop::instance().timeout());
	}

	QFETCH(int, parserError);
	if (parserError == PlaylistParser::NoError) {
		QCOMPARE(spyFilesFound.count(), 1);
	} else {
		//If file couldn't be opened then no filesFound() signal
		//has been sent
		QCOMPARE(spyFilesFound.count(), 0);
	}

	QCOMPARE(spyFinished.count(), 1);
}

void PlaylistParserTest::filesFound(const QList<MediaInfo> & files) {
	QFETCH(QStringList, filesFound);

	QCOMPARE(files.size(), filesFound.size());

	for (int i = 0; i < filesFound.size(); i++) {
		QString file(files[i].fileName());
		QString fileFound(filesFound[i]);
		QVERIFY(file.endsWith(fileFound));
	}
}

void PlaylistParserTest::finished(PlaylistParser::Error error, int timeElapsed) {
	QFETCH(int, parserTimeElapsed);

	QFETCH(int, parserError);
	QCOMPARE(parserError, static_cast<int>(error));

	QFETCH(int, parserFileError);
	int fileError = _parser->file().error();
	QCOMPARE(parserFileError, fileError);

	qDebug() << __FUNCTION__ << "timeElapsed:" << timeElapsed;

	QTestEventLoop::instance().exitLoop();
}
