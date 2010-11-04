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

#include "PlaylistParserTest.h"

#include <PlaylistParser/PlaylistParser.h>
#include <PlaylistParser/PlaylistParserLogger.h>

#include <MediaInfoFetcher/MediaInfo.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QList>

QTEST_MAIN(PlaylistParserTest)

void disableMessageOutput(QtMsgType type, const char * msg) {
	Q_UNUSED(type);
	Q_UNUSED(msg);
}

void PlaylistParserTest::initTestCase() {
	//Disable qDebug() and friends
	qInstallMsgHandler(disableMessageOutput);
	///

	_parser = new PlaylistReader(this);
	connect(_parser, SIGNAL(filesFound(const QList<MediaInfo> &)),
		SLOT(filesFound(const QList<MediaInfo> &)));
	connect(_parser, SIGNAL(finished(PlaylistParser::Error, int)),
		SLOT(finished(PlaylistParser::Error, int)));
}

void PlaylistParserTest::cleanupTestCase() {
	//Wait for the user to press a key
	//getchar();
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

	//Non existing file
	QTest::newRow("Non existing File") << "Non existing File"
		<< QList<MediaInfo>()
		<< static_cast<int>(PlaylistParser::FileError)
		<< static_cast<int>(QFile::OpenError)
		<< 0;

	//Unsupported Format
	QTest::newRow("Unsupported Format") << ":/TheKMPlayer-2.9.4/test.KPL"
		<< QList<MediaInfo>()
		<< static_cast<int>(PlaylistParser::UnsupportedFormatError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	loadPlaylistM3U();
	loadPlaylistM3U8();
	loadPlaylistWPL();
	loadPlaylistPLS();
	loadPlaylistASX();
	loadPlaylistXSPF();
	loadPlaylistCUE();
}

QList<MediaInfo> localM3UPlaylist() {
	//C:\\ and \\ are equivalent from a Qt point of vue
	return QList<MediaInfo>()
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\01-Do Whats Good For Me.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\02-No Limit.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\03-Get Ready For This.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\04-Twilight Zone.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\05-No One.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\06-Jump For Joy.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\07-Tribal Dance.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\08-Magic Friend.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\09-Workaholic.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\10-Let The Beat Control Your Body.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\11-Nothing Like The Rain.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\12-Spread Your Love.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\13-The Real Thing.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\14-Here I Go.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\15-Maximum Overdrive.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\2 Unlimited - The Very Best Of 2 Unlimited\\16-Faces.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Blondie - The Best of\\01 Blondie - Heart Of Glass.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Blondie - The Best of\\02 Blondie - Dreaming.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Blondie - The Best of\\03 Blondie - The Tide Is High.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Blondie - The Best of\\04 Blondie - In The Flesh.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Blondie - The Best of\\05 Blondie - Sunday Girl.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Blondie - The Best of\\06 Blondie - Hanging On The Telephone.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Blondie - The Best of\\07 Blondie - Rapture.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Blondie - The Best of\\08 Blondie - One Way Or Another.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Blondie - The Best of\\09 Blondie - (I'm Always Touched By Your) Presence Dear.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Blondie - The Best of\\10 Blondie - Call Me (Theme From American Gigolo).mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Blondie - The Best of\\11 Blondie - Atomic.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Blondie - The Best of\\12 Blondie - Rip Her To Shreds.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\01  Madcon - Beggin'.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\02  Madcon - Back On The Road [With Paperboys].mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\03  Madcon - Liar.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\04  Madcon - Hard To Read [With Noora].mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\05  Madcon - Life's Too Short.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\06  Madcon - The Way We Do Thangs [With Timbuktu].mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\07  Madcon - Blessed.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\08  Madcon - Suda Suda [With El Axel].mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\09  Madcon - Let It Be Known.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\10  Madcon - Let's Dance Instead.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\11  Madcon - Dandelion.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\12  Madcon - Pride And Prejudice [With Sofian].mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\13  Madcon - Me And My Brother.mp3")
		<< MediaInfo("\\Documents and Settings\\tanguy_k\\My Documents\\Downloads\\Madcon-So Dark The Con Of Man[2008][CD+SkidVid_XviD+Cov]\\14  Madcon - Loose [Bonus].mp3");
}

void PlaylistParserTest::loadPlaylistM3U() {
	//Foobar
	QTest::newRow("Foobar M3U") << ":/foobar2000-0.9.6.7/test.m3u"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//Foobar
	QTest::newRow("Foobar M3U8") << ":/foobar2000-0.9.6.7/test.m3u8"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//TheKMPlayer
	QTest::newRow("TheKMPlayer M3U") << ":/TheKMPlayer-2.9.4/test.M3U"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//Winamp
	QTest::newRow("Winamp M3U") << ":/Winamp-5.531/test.m3u"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//Winamp
	QTest::newRow("Winamp M3U8") << ":/Winamp-5.531/test.m3u8"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//WindowsMediaPlayer
	QTest::newRow("WindowsMediaPlayer M3U") << ":/WindowsMediaPlayer-9.0/test.m3u"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//Europe1
	QTest::newRow("Europe1 M3U") << ":/fromInternet/webradio/europe1/europe1.m3u"
		<< (QList<MediaInfo>()
			<< MediaInfo("http://vipicecast.yacast.net/europe1"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//Live9
	QTest::newRow("Live9 M3U") << ":/fromInternet/webradio/live9/192.m3u"
		<< (QList<MediaInfo>()
			<< MediaInfo("http://stream192.live9.fr:8050")
			<< MediaInfo("http://stream192.live9.fr:8050")
			<< MediaInfo("http://stream192.live9.fr:8050")
			<< MediaInfo("http://stream192.live9.fr:8050")
			<< MediaInfo("http://stream192.live9.fr:8050"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//France Inter
	QTest::newRow("France Inter M3U") << ":/fromInternet/webradio/franceinter/france_inter_mp3-128k.m3u"
		<< (QList<MediaInfo>()
			<< MediaInfo("http://mp3.live.tv-radio.com/franceinter/all/franceinterhautdebit.mp3"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//Radio Nova
	QTest::newRow("Radio Nova M3U") << ":/fromInternet/webradio/radionova/radionova.m3u"
		<< (QList<MediaInfo>()
			<< MediaInfo("http://broadcast.infomaniak.net:80/radionova-high.mp3"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;
}

void PlaylistParserTest::loadPlaylistM3U8() {
}

void PlaylistParserTest::loadPlaylistWPL() {
	//WindowsMediaPlayer
	/*QTest::newRow("WindowsMediaPlayer WPL") << ":/WindowsMediaPlayer-9.0/test.wpl"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;*/
}

void PlaylistParserTest::loadPlaylistPLS() {
	//France Inter
	QTest::newRow("France Inter PLS") << ":/fromInternet/webradio/franceinter/france_inter_mp3-128k.pls"
		<< (QList<MediaInfo>()
			<< MediaInfo("http://mp3.live.tv-radio.com/franceinter/all/franceinterhautdebit.mp3")
			<< MediaInfo("http://mp3.live.tv-radio.com/franceinter/all/franceinterhautdebit.mp3"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;
}

void PlaylistParserTest::loadPlaylistASX() {
	//Live9
	QTest::newRow("Live9 ASX") << ":/fromInternet/webradio/live9/192.asx"
		<< (QList<MediaInfo>()
			<< MediaInfo("mms://wm.live9.fr/live9live"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//OUI FM
	QTest::newRow("OUI FM ASX") << ":/fromInternet/webradio/ouifm/Ouifm-hautdebit-wmp.asx"
		<< (QList<MediaInfo>()
			<< MediaInfo("http://www.ouifm.fr/player/IntroPubOUIFM.mp3")
			<< MediaInfo("http://broadcast.infomaniak.net:80/ouifm-high.mp3"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//Virgin Radio
	QTest::newRow("Virgin Radio ASX") << ":/fromInternet/webradio/virginradio/virgin.asx"
		<< (QList<MediaInfo>()
			<< MediaInfo("mms://viplagardere.yacast.net/encodereurope2"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;

	//Europe1
	QTest::newRow("Europe1 ASX") << ":/fromInternet/webradio/europe1/europe1.asx"
		<< (QList<MediaInfo>()
			<< MediaInfo("mms://viplagardere.yacast.net/encodereurope1"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;
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
		//There can be more than 1 filesFound() signal
		QCOMPARE(spyFilesFound.count(), 1);
	} else {
		//If file couldn't be opened then no filesFound() signal
		//has been sent
		QCOMPARE(spyFilesFound.count(), 0);
	}

	//Only 1 finished() signal
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
	//QFETCH(int, parserTimeElapsed);

	QFETCH(int, parserError);
	QCOMPARE(parserError, static_cast<int>(error));

	QFETCH(int, parserFileError);
	int fileError = _parser->file().error();
	QCOMPARE(parserFileError, fileError);

	PlaylistParserDebug() << "timeElapsed:" << timeElapsed;

	QTestEventLoop::instance().exitLoop();
}
