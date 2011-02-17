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

#include "PlaylistReaderTest.h"

#include <PlaylistParser/PlaylistParser.h>
#include <PlaylistParser/PlaylistParserLogger.h>

#include <MediaInfoFetcher/MediaInfo.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QList>

QTEST_MAIN(PlaylistReaderTest)

void disableMessageOutput(QtMsgType type, const char * msg) {
	Q_UNUSED(type);
	Q_UNUSED(msg);
}

void PlaylistReaderTest::initTestCase() {
	//Disable qDebug() and friends
	qInstallMsgHandler(disableMessageOutput);
	///

	//Playlist reader
	_reader = new PlaylistReader(this);
	connect(_reader, SIGNAL(filesFound(const QList<MediaInfo> &)),
		SLOT(filesFound(const QList<MediaInfo> &)));
	connect(_reader, SIGNAL(finished(PlaylistParser::Error, int)),
		SLOT(finished(PlaylistParser::Error, int)));
}

void PlaylistReaderTest::cleanupTestCase() {
}

void PlaylistReaderTest::init() {
}

void PlaylistReaderTest::cleanup() {
}

void PlaylistReaderTest::loadPlaylist_data() {
	QTest::addColumn<QString>("playlistFileName");
	QTest::addColumn<QList<MediaInfo> >("filesToFind");
	QTest::addColumn<int>("parserError");
	QTest::addColumn<int>("parserTimeElapsed");

	//Non existing file
	QTest::newRow("Non existing File") << "Non existing File.m3u"
		<< QList<MediaInfo>()
		<< static_cast<int>(PlaylistParser::IOError)
		<< 0;

	//Unsupported format: The KMPlayer proprietary format
	QTest::newRow("Unsupported Format") << ":/TheKMPlayer/test.KPL"
		<< QList<MediaInfo>()
		<< static_cast<int>(PlaylistParser::UnsupportedFormatError)
		<< 0;

	loadPlaylistM3U();
	loadPlaylistWPL();
	loadPlaylistPLS();
	loadPlaylistASX();
	loadPlaylistXSPF();
	loadPlaylistCUE();
}

QList<MediaInfo> localM3UPlaylist() {
	return QList<MediaInfo>()
		<< MediaInfo("C:\\1.mp3")
		<< MediaInfo("C:\\2.mp3")
		<< MediaInfo("C:\\3.mp3")
		<< MediaInfo("C:\\4.mp3")
		<< MediaInfo("C:\\5.mp3");
}

QList<MediaInfo> localM3UPlaylist_WithMetaDatas() {
	QList<MediaInfo> mediaInfoList;

	MediaInfo m1("C:\\1.mp3");
	m1.setDurationSecs(0);
	mediaInfoList << m1;

	MediaInfo m2("C:\\2.mp3");
	m2.setMetaData(MediaInfo::Title, "Title2");
	m2.setDurationSecs(0);
	mediaInfoList << m2;

	MediaInfo m3("C:\\3.mp3");
	//m3.setMetaData(MediaInfo::Title, "Artist3 - 3");
	m3.setDurationSecs(0);
	mediaInfoList << m3;

	MediaInfo m4("C:\\4.mp3");
	m4.setMetaData(MediaInfo::Title, "Artist4 - Title4");
	m4.setDurationSecs(0);
	mediaInfoList << m4;

	MediaInfo m5("C:\\5.mp3");
	m5.setMetaData(MediaInfo::Title, "Artist5 - Title5");
	m5.setDurationSecs(0);
	mediaInfoList << m5;

	return mediaInfoList;
}

QList<MediaInfo> localM3UPlaylist_NoRoot() {
	return QList<MediaInfo>()
		<< MediaInfo("\\1.mp3")
		<< MediaInfo("\\2.mp3")
		<< MediaInfo("\\3.mp3")
		<< MediaInfo("\\4.mp3")
		<< MediaInfo("\\5.mp3");
}

QList<MediaInfo> localM3UPlaylist_Relative() {
	return QList<MediaInfo>()
		<< MediaInfo("..\\mp3\\1.mp3")
		<< MediaInfo("..\\mp3\\2.mp3")
		<< MediaInfo("..\\mp3\\3.mp3")
		<< MediaInfo("..\\mp3\\4.mp3")
		<< MediaInfo("..\\mp3\\5.mp3");
}

QList<MediaInfo> localXSPFPlaylist() {
	QList<MediaInfo> mediaInfoList;

	MediaInfo m1("file:///C:/1.mp3");
	mediaInfoList << m1;

	MediaInfo m2("file:///C:/2.mp3");
	m2.setMetaData(MediaInfo::TrackNumber, 2);
	//m2.setMetaData(MediaInfo::DiscNumber, 2);
	//m2.setMetaData(MediaInfo::BPM, 2);
	m2.setMetaData(MediaInfo::Title, "Title2");
	m2.setMetaData(MediaInfo::Album, "Album2");
	//m2.setMetaData(MediaInfo::AlbumArtist, "Album Artist2");
	//m2.setMetaData(MediaInfo::Year, 2011);
	//m2.setMetaData(MediaInfo::Genre, "Genre2");
	m2.setMetaData(MediaInfo::Comment, "Comment2");
	//m2.setMetaData(MediaInfo::Composer, "Composer2");
	//m2.setMetaData(MediaInfo::Publisher, "Publisher2");
	mediaInfoList << m2;

	MediaInfo m3("file:///C:/3.mp3");
	m3.setMetaData(MediaInfo::TrackNumber, 3);
	//m3.setMetaData(MediaInfo::DiscNumber, 3);
	//m3.setMetaData(MediaInfo::BPM, 3);
	m3.setMetaData(MediaInfo::Artist, "Artist3");
	m3.setMetaData(MediaInfo::Album, "Album3");
	//m3.setMetaData(MediaInfo::AlbumArtist, "Album Artist3");
	//m3.setMetaData(MediaInfo::Year, 2011);
	//m3.setMetaData(MediaInfo::Genre, "Genre3");
	m3.setMetaData(MediaInfo::Comment, "Comment3");
	//m3.setMetaData(MediaInfo::Composer, "Composer3");
	//m3.setMetaData(MediaInfo::Publisher, "Publisher3");
	mediaInfoList << m3;

	MediaInfo m4("file:///C:/4.mp3");
	m4.setMetaData(MediaInfo::TrackNumber, 4);
	//m4.setMetaData(MediaInfo::DiscNumber, 4);
	//m4.setMetaData(MediaInfo::BPM, 4);
	m4.setMetaData(MediaInfo::Title, "Title4");
	m4.setMetaData(MediaInfo::Artist, "Artist4");
	m4.setMetaData(MediaInfo::Album, "Album4");
	//m4.setMetaData(MediaInfo::AlbumArtist, "Album Artist4");
	//m4.setMetaData(MediaInfo::Year, 2011);
	//m4.setMetaData(MediaInfo::Genre, "Genre4");
	m4.setMetaData(MediaInfo::Comment, "Comment4");
	//m4.setMetaData(MediaInfo::Composer, "Composer4");
	//m4.setMetaData(MediaInfo::Publisher, "Publisher4");
	mediaInfoList << m4;

	MediaInfo m5("file:///C:/5.mp3");
	m5.setMetaData(MediaInfo::TrackNumber, 5);
	//m5.setMetaData(MediaInfo::DiscNumber, 5);
	//m5.setMetaData(MediaInfo::BPM, 5);
	m5.setMetaData(MediaInfo::Title, "Title5");
	m5.setMetaData(MediaInfo::Artist, "Artist5");
	m5.setMetaData(MediaInfo::Album, "Album5");
	//m5.setMetaData(MediaInfo::AlbumArtist, "Album Artist5");
	//m5.setMetaData(MediaInfo::Year, 2011);
	//m5.setMetaData(MediaInfo::Genre, "Genre5");
	m5.setMetaData(MediaInfo::Comment, "Comment5");
	//m5.setMetaData(MediaInfo::Composer, "Composer5");
	//m5.setMetaData(MediaInfo::Publisher, "Publisher5");
	mediaInfoList << m5;

	return mediaInfoList;
}

void PlaylistReaderTest::loadPlaylistM3U() {
	//Foobar
	QTest::newRow("Foobar M3U") << ":/foobar2000/test.m3u"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//Foobar
	QTest::newRow("Foobar M3U8") << ":/foobar2000/test.m3u8"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//TheKMPlayer
	QTest::newRow("TheKMPlayer M3U") << ":/TheKMPlayer/test.M3U"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//VLC
	QTest::newRow("VLC M3U") << ":/VLC/test.m3u"
		<< localM3UPlaylist_WithMetaDatas()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//VLC
	QTest::newRow("VLC M3U8") << ":/VLC/test.m3u8"
		<< localM3UPlaylist_WithMetaDatas()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//Winamp
	QTest::newRow("Winamp M3U") << ":/Winamp/test.m3u"
		<< localM3UPlaylist_WithMetaDatas()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//Winamp
	QTest::newRow("Winamp M3U8") << ":/Winamp/test.m3u8"
		<< localM3UPlaylist_WithMetaDatas()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//Winamp
	QTest::newRow("Winamp M3U") << ":/Winamp/test-noroot.m3u"
		<< localM3UPlaylist_NoRoot()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//WindowsMediaPlayer
	QTest::newRow("WindowsMediaPlayer M3U") << ":/WindowsMediaPlayer/test.m3u"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//WindowsMediaPlayer
	QTest::newRow("WindowsMediaPlayer M3U") << ":/WindowsMediaPlayer/test-relative.m3u"
		<< localM3UPlaylist_Relative()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//Europe1
	QTest::newRow("Europe1 M3U") << ":/Internet/webradio/europe1/europe1.m3u"
		<< (QList<MediaInfo>()
			<< MediaInfo("http://vipicecast.yacast.net/europe1"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//Live9
	QTest::newRow("Live9 M3U") << ":/Internet/webradio/live9/192.m3u"
		<< (QList<MediaInfo>()
			<< MediaInfo("http://stream192.live9.fr:8050")
			<< MediaInfo("http://stream192.live9.fr:8050")
			<< MediaInfo("http://stream192.live9.fr:8050")
			<< MediaInfo("http://stream192.live9.fr:8050")
			<< MediaInfo("http://stream192.live9.fr:8050"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//France Inter
	QTest::newRow("France Inter M3U") << ":/Internet/webradio/franceinter/france_inter_mp3-128k.m3u"
		<< (QList<MediaInfo>()
			<< MediaInfo("http://mp3.live.tv-radio.com/franceinter/all/franceinterhautdebit.mp3"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//Radio Nova
	QTest::newRow("Radio Nova M3U") << ":/Internet/webradio/radionova/radionova.m3u"
		<< (QList<MediaInfo>()
			<< MediaInfo("http://broadcast.infomaniak.net:80/radionova-high.mp3"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;
}

void PlaylistReaderTest::loadPlaylistWPL() {
	//WindowsMediaPlayer
	QTest::newRow("WindowsMediaPlayer WPL") << ":/WindowsMediaPlayer/test.wpl"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//WindowsMediaPlayer
	QTest::newRow("WindowsMediaPlayer WPL") << ":/WindowsMediaPlayer/test-relative.wpl"
		<< localM3UPlaylist_Relative()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;
}

void PlaylistReaderTest::loadPlaylistPLS() {
	//France Inter
	QTest::newRow("France Inter PLS") << ":/Internet/webradio/franceinter/france_inter_mp3-128k.pls"
		<< (QList<MediaInfo>()
			<< MediaInfo("http://mp3.live.tv-radio.com/franceinter/all/franceinterhautdebit.mp3")
			<< MediaInfo("http://mp3.live.tv-radio.com/franceinter/all/franceinterhautdebit.mp3"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//TheKMPlayer
	QTest::newRow("TheKMPlayer PLS") << ":/TheKMPlayer/test.PLS"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//Winamp
	QTest::newRow("Winamp PLS") << ":/Winamp/test.pls"
		<< localM3UPlaylist_WithMetaDatas()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;
}

void PlaylistReaderTest::loadPlaylistASX() {
	//Live9
	QTest::newRow("Live9 ASX") << ":/Internet/webradio/live9/192.asx"
		<< (QList<MediaInfo>()
			<< MediaInfo("mms://wm.live9.fr/live9live"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//OUI FM
	QTest::newRow("OUI FM ASX") << ":/Internet/webradio/ouifm/Ouifm-hautdebit-wmp.asx"
		<< (QList<MediaInfo>()
			<< MediaInfo("http://www.ouifm.fr/player/IntroPubOUIFM.mp3")
			<< MediaInfo("http://broadcast.infomaniak.net:80/ouifm-high.mp3"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//Virgin Radio
	QTest::newRow("Virgin Radio ASX") << ":/Internet/webradio/virginradio/virgin.asx"
		<< (QList<MediaInfo>()
			<< MediaInfo("mms://viplagardere.yacast.net/encodereurope2"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//Europe1
	QTest::newRow("Europe1 ASX") << ":/Internet/webradio/europe1/europe1.asx"
		<< (QList<MediaInfo>()
			<< MediaInfo("mms://viplagardere.yacast.net/encodereurope1"))
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//TheKMPlayer
	//FIXME Qt bug, tested with Qt 4.7.0 MinGW Windows XP
	//"<Ref href = "C:\1.mp3"/>"
	//will return "C:C:\1.mp3.mp3"
	/*QTest::newRow("TheKMPlayer ASX") << ":/TheKMPlayer/test.ASX"
		<< localM3UPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< static_cast<int>(QFile::NoError)
		<< 0;*/
}

void PlaylistReaderTest::loadPlaylistXSPF() {
	//Foobar
	QTest::newRow("Foobar XSPF") << ":/foobar2000/test.xspf"
		<< localXSPFPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//VLC
	QTest::newRow("VLC XSPF") << ":/VLC/test.xspf"
		<< localXSPFPlaylist()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;
}

void PlaylistReaderTest::loadPlaylistCUE() {
}

void PlaylistReaderTest::loadPlaylist() {
	QSignalSpy spyFilesFound(_reader, SIGNAL(filesFound(const QList<MediaInfo> &)));
	QSignalSpy spyFinished(_reader, SIGNAL(finished(PlaylistParser::Error, int)));

	QFETCH(QString, playlistFileName);
	_reader->load(playlistFileName);

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

void PlaylistReaderTest::filesFound(const QList<MediaInfo> & files) {
	QFETCH(QList<MediaInfo>, filesToFind);

	QCOMPARE(files.size(), filesToFind.size());

	for (int i = 0; i < filesToFind.size(); i++) {
		//Compare the filenames
		QCOMPARE(filesToFind[i].fileName(), files[i].fileName());

		//Compare the metadatas if any
		for (int metaData = MediaInfo::MIN; metaData <= MediaInfo::MAX; metaData++) {
			if (filesToFind[i].metaDataValue((MediaInfo::MetaData) metaData).isValid()) {
				QCOMPARE(filesToFind[i].metaDataValue((MediaInfo::MetaData) metaData),
					files[i].metaDataValue((MediaInfo::MetaData) metaData));
			}
		}

		//Compare the durations
		if (filesToFind[i].durationSecs() != -1) {
			QCOMPARE(filesToFind[i].durationSecs(), files[i].durationSecs());
		}
	}
}

void PlaylistReaderTest::finished(PlaylistParser::Error error, int timeElapsed) {
	//QFETCH(int, parserTimeElapsed);

	QFETCH(int, parserError);
	QCOMPARE(parserError, static_cast<int>(error));

	PlaylistParserDebug() << "timeElapsed:" << timeElapsed;

	QTestEventLoop::instance().exitLoop();
}
