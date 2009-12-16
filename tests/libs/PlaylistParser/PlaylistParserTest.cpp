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
#include <QtCore/QList>

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

	QTest::addColumn<QList<MediaInfo> >("filesFound");
	QTest::addColumn<int>("parserError");
	QTest::addColumn<int>("parserFileError");
	QTest::addColumn<int>("parserTimeElapsed");

	loadPlaylistM3U();
	loadPlaylistM3U8();
	loadPlaylistWPL();
	loadPlaylistPLS();
	loadPlaylistASX();
	loadPlaylistXSPF();
	loadPlaylistCUE();

	/*//Non existing file
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
	*/
}

void PlaylistParserTest::loadPlaylistM3U() {
	MediaInfoList mediaInfoList;
	MediaInfo mediaInfo;

	mediaInfo.clear();
	mediaInfo.setFileName("C:\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\01-Do Whats Good For Me.mp3");
	mediaInfoList += mediaInfo;

	mediaInfo.clear();
	mediaInfo.setFileName("C:\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\02-No Limit.mp3");
	mediaInfoList += mediaInfo;

	QTest::newRow("M3U") << ":/foobar2000-0.9.6.7/test.m3u"
		<< mediaInfoList
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;
}

void PlaylistParserTest::loadPlaylistM3U8() {
}

void PlaylistParserTest::loadPlaylistWPL() {
}

void PlaylistParserTest::loadPlaylistPLS() {
}

void PlaylistParserTest::loadPlaylistASX() {
}

void PlaylistParserTest::loadPlaylistXSPF() {
}

void PlaylistParserTest::loadPlaylistCUE() {
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
	QFETCH(QList<MediaInfo>, filesFound);

	QCOMPARE(files.size(), filesFound.size());

	for (int i = 0; i < filesFound.size(); i++) {
		QCOMPARE(filesFound[i], files[i]);
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
