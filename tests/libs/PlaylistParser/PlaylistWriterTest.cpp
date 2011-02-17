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

#include "PlaylistWriterTest.h"

#include <PlaylistParser/PlaylistParser.h>
#include <PlaylistParser/PlaylistParserLogger.h>

#include <MediaInfoFetcher/MediaInfo.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QList>

QTEST_MAIN(PlaylistWriterTest)

void disableMessageOutput(QtMsgType type, const char * msg) {
	Q_UNUSED(type);
	Q_UNUSED(msg);
}

void PlaylistWriterTest::initTestCase() {
	//Disable qDebug() and friends
	qInstallMsgHandler(disableMessageOutput);
	///

	//Playlist writer
	_writer = new PlaylistWriter(this);
	connect(_writer, SIGNAL(finished(PlaylistParser::Error, int)),
		SLOT(writerFinished(PlaylistParser::Error, int)));

	//Playlist reader
	_reader = new PlaylistReader(this);
	connect(_reader, SIGNAL(filesFound(const QList<MediaInfo> &)),
		SLOT(readerFilesFound(const QList<MediaInfo> &)));
	connect(_reader, SIGNAL(finished(PlaylistParser::Error, int)),
		SLOT(readerFinished(PlaylistParser::Error, int)));
}

void PlaylistWriterTest::cleanupTestCase() {
}

void PlaylistWriterTest::init() {
}

void PlaylistWriterTest::cleanup() {
}

QList<MediaInfo> mediaInfoListToSave_WPL() {
	return QList<MediaInfo>()
		<< MediaInfo("1.mp3")
		<< MediaInfo("2.mp3")
		<< MediaInfo("3.mp3")
		<< MediaInfo("4.mp3")
		<< MediaInfo("5.mp3");
}

QList<MediaInfo> mediaInfoListToSave_ASX() {
	//FIXME Qt bug, tested with Qt 4.7.0 MinGW Windows XP
	//xml.readElementText() returns lower case text
	//So all titles must be lower case in our tests

	QList<MediaInfo> mediaInfoList;

	MediaInfo m1("1.mp3");
	mediaInfoList << m1;

	MediaInfo m2("2.mp3");
	m2.setMetaData(MediaInfo::Title, "title2");
	mediaInfoList << m2;

	MediaInfo m3("3.mp3");
	mediaInfoList << m3;

	MediaInfo m4("4.mp3");
	m4.setMetaData(MediaInfo::Title, "title4");
	mediaInfoList << m4;

	MediaInfo m5("5.mp3");
	m5.setMetaData(MediaInfo::Title, "title5");
	mediaInfoList << m5;

	return mediaInfoList;
}

QList<MediaInfo> mediaInfoListToSave_M3U() {
	QList<MediaInfo> mediaInfoList;

	MediaInfo m1("1.mp3");
	mediaInfoList << m1;

	MediaInfo m2("2.mp3");
	m2.setMetaData(MediaInfo::Title, "Title2");
	mediaInfoList << m2;

	MediaInfo m3("3.mp3");
	m3.setMetaData(MediaInfo::Title, "Artist3");
	mediaInfoList << m3;

	MediaInfo m4("4.mp3");
	m4.setMetaData(MediaInfo::Title, "Artist4 - Title4");
	mediaInfoList << m4;

	MediaInfo m5("5.mp3");
	m5.setMetaData(MediaInfo::Title, "Artist5 - Title5");
	mediaInfoList << m5;

	return mediaInfoList;
}

QList<MediaInfo> mediaInfoListToSave() {
	QList<MediaInfo> mediaInfoList;

	MediaInfo m1("1.mp3");
	mediaInfoList << m1;

	MediaInfo m2("2.mp3");
	m2.setMetaData(MediaInfo::Title, "Title2");
	mediaInfoList << m2;

	MediaInfo m3("3.mp3");
	m3.setMetaData(MediaInfo::Artist, "Artist3");
	mediaInfoList << m3;

	MediaInfo m4("4.mp3");
	m4.setMetaData(MediaInfo::Title, "Title4");
	m4.setMetaData(MediaInfo::Artist, "Artist4");
	mediaInfoList << m4;

	MediaInfo m5("5.mp3");
	m5.setMetaData(MediaInfo::Title, "Title5");
	m5.setMetaData(MediaInfo::Artist, "Artist5");
	mediaInfoList << m5;

	return mediaInfoList;
}

void PlaylistWriterTest::savePlaylist_data() {
	QTest::addColumn<QString>("playlistFileName");
	QTest::addColumn<QList<MediaInfo> >("mediaList");
	QTest::addColumn<int>("parserError");
	QTest::addColumn<int>("parserTimeElapsed");

	//Unsupported format: The KMPlayer proprietary format
	QTest::newRow("Unsupported Format") << "PlaylistWriterTest.KPL"
		<< QList<MediaInfo>()
		<< static_cast<int>(PlaylistParser::UnsupportedFormatError)
		<< 0;

	//ASX
	QTest::newRow("ASX") << "PlaylistWriterTest.asx"
		<< mediaInfoListToSave_ASX()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//M3U
	QTest::newRow("M3U") << "PlaylistWriterTest.m3u"
		<< mediaInfoListToSave_M3U()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//PLS
	QTest::newRow("PLS") << "PlaylistWriterTest.pls"
		<< mediaInfoListToSave_M3U()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//WPL
	QTest::newRow("WPL") << "PlaylistWriterTest.wpl"
		<< mediaInfoListToSave_WPL()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;

	//XSPF
	QTest::newRow("XSPF") << "PlaylistWriterTest.xspf"
		<< mediaInfoListToSave()
		<< static_cast<int>(PlaylistParser::NoError)
		<< 0;
}

void PlaylistWriterTest::savePlaylist() {
	QSignalSpy spyFinished(_writer, SIGNAL(finished(PlaylistParser::Error, int)));

	QFETCH(QString, playlistFileName);
	QFETCH(QList<MediaInfo>, mediaList);
	_writer->save(playlistFileName, mediaList);

	//finished() signal might be already sent by save() method
	//if the file couldn't be opened
	if (spyFinished.count() == 0) {
		QTestEventLoop::instance().enterLoop(30);
		QVERIFY(!QTestEventLoop::instance().timeout());
	}

	//Only 1 finished() signal
	QCOMPARE(spyFinished.count(), 1);
	QFETCH(int, parserError);
	if (parserError == PlaylistParser::NoError) {
		loadPlaylist(playlistFileName);
	} else {
	}
}

void PlaylistWriterTest::writerFinished(PlaylistParser::Error error, int timeElapsed) {
	//QFETCH(int, parserTimeElapsed);

	QFETCH(int, parserError);
	QCOMPARE(parserError, static_cast<int>(error));

	PlaylistParserDebug() << "timeElapsed:" << timeElapsed;

	QTestEventLoop::instance().exitLoop();
}

void PlaylistWriterTest::loadPlaylist(const QString & playlistFileName) {
	QSignalSpy spyFilesFound(_reader, SIGNAL(filesFound(const QList<MediaInfo> &)));
	QSignalSpy spyFinished(_reader, SIGNAL(finished(PlaylistParser::Error, int)));

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

void PlaylistWriterTest::readerFilesFound(const QList<MediaInfo> & files) {
	QFETCH(QList<MediaInfo>, mediaList);

	QCOMPARE(files.size(), mediaList.size());

	for (int i = 0; i < mediaList.size(); i++) {
		//Compare the filenames
		QCOMPARE(mediaList[i].fileName(), files[i].fileName());

		//Compare the metadatas if any
		for (int metaData = MediaInfo::MIN; metaData <= MediaInfo::MAX; metaData++) {
			if (mediaList[i].metaDataValue((MediaInfo::MetaData) metaData).isValid()) {
				QCOMPARE(mediaList[i].metaDataValue((MediaInfo::MetaData) metaData),
					files[i].metaDataValue((MediaInfo::MetaData) metaData));
			}
		}

		//Compare the durations
		if (mediaList[i].durationSecs() != -1) {
			QCOMPARE(mediaList[i].durationSecs(), files[i].durationSecs());
		}
	}
}

void PlaylistWriterTest::readerFinished(PlaylistParser::Error error, int timeElapsed) {
	//QFETCH(int, parserTimeElapsed);

	QFETCH(int, parserError);
	QCOMPARE(parserError, static_cast<int>(error));

	PlaylistParserDebug() << "timeElapsed:" << timeElapsed;

	QTestEventLoop::instance().exitLoop();
}
