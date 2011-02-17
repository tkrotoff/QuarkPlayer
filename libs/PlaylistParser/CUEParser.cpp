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

#include "CUEParser.h"

#include "Util.h"
#include "PlaylistParserLogger.h"

#include <MediaInfoFetcher/MediaInfo.h>
#include <TkUtil/TkFile.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QtCore/QDate>
#include <QtCore/QTextStream>

static const char * CUE_QUOTE = "\"";
static const char * CUE_EOL = "\r\n";

CUEParser::CUEParser(QObject * parent)
	: IPlaylistParserImpl(parent) {

	_stop = false;
}

CUEParser::~CUEParser() {
	stop();
}

QStringList CUEParser::fileExtensions() const {
	QStringList extensions;
	extensions << "cue";
	return extensions;
}

void CUEParser::stop() {
	_stop = true;
}

bool CUEParser::load(const QString & location) {
	QIODevice * device = Util::openLocationReadMode(location);
	if (!device) {
		return false;
	}

	_stop = false;

	QList<MediaInfo> files;

	//See http://regexlib.com/DisplayPatterns.aspx

	//REM GENRE "Alternative rock"
	//REM GENRE Pop
	QRegExp rx_genre("^REM GENRE (.*)$");
	//REM DATE 1994
	QRegExp rx_date("^REM DATE (\\d+)$");
	//PERFORMER "Weezer"
	QRegExp rx_root_performer("^PERFORMER \"(.*)\"$");
	//TITLE "Weezer (The Blue Album)"
	QRegExp rx_root_title("^TITLE \"(.*)\"$");
	//FILE "01. My Name Is Jonas - [Weezer] .wav" WAVE
	QRegExp rx_file("^FILE \"(.*)\"");
	//  TRACK 01 AUDIO
	QRegExp rx_track("^  TRACK (\\d+)");
	//    TITLE "No One Else"
	QRegExp rx_title("^    TITLE \"(.*)\"$");
	//    PERFORMER "Weezer"
	QRegExp rx_performer("^    PERFORMER \"(.*)\"$");
	//    INDEX 01 00:00:00
	QRegExp rx_index("^    INDEX 01 (.*)$");

	QString path(QFileInfo(location).path());

	QTextStream stream(device);

	MediaInfo mediaInfo;

	//Root elements
	QString genre;
	QDate date;
	QString albumArtist;
	QString album;
	///

	QString filename;

	while (!stream.atEnd() && !_stop) {
		//Line of text, don't use trimmed()
		//since CUE sheet files contain indentation spaces
		QString line(stream.readLine());

		if (line.isEmpty()) {
			//Do nothing
		}

		else if (rx_genre.indexIn(line) != -1) {
			genre = rx_genre.cap(1);
		}

		else if (rx_date.indexIn(line) != -1) {
			date = QDate::fromString(rx_date.cap(1));
		}

		else if (rx_root_performer.indexIn(line) != -1) {
			albumArtist = rx_root_performer.cap(1);
		}

		else if (rx_root_title.indexIn(line) != -1) {
			album = rx_root_title.cap(1);
		}

		else if (rx_file.indexIn(line) != -1) {
			filename = rx_file.cap(1);
		}

		else if (rx_track.indexIn(line) != -1) {
			QString track(rx_track.cap(1));
			mediaInfo.setMetaData(MediaInfo::TrackNumber, track.toInt());
		}

		else if (rx_title.indexIn(line) != -1) {
			QString title(rx_title.cap(1));
			mediaInfo.setMetaData(MediaInfo::Title, title);
		}

		else if (rx_performer.indexIn(line) != -1) {
			QString performer(rx_performer.cap(1));
			mediaInfo.setMetaData(MediaInfo::Artist, performer);
		}

		else if (rx_index.indexIn(line) != -1) {
			QString index(rx_index.cap(1));
			mediaInfo.setCueStartIndex(index);

			if (!files.isEmpty()) {
				//Now we know the CUE end index from the previous media
				files.last().setCueEndIndex(index);
			}

			mediaInfo.setMetaData(MediaInfo::Genre, genre);
			mediaInfo.setMetaData(MediaInfo::Year, date);
			mediaInfo.setMetaData(MediaInfo::AlbumArtist, albumArtist);
			mediaInfo.setMetaData(MediaInfo::Album, album);

			if (MediaInfo::isUrl(filename)) {
				mediaInfo.setFileName(filename);
			} else {
				mediaInfo.setFileName(Util::canonicalFilePath(path, filename));
			}

			if (!mediaInfo.fileName().isEmpty()) {
				//Add file to the list of files
				files << mediaInfo;

				//Clear the MediaInfo for the next lines
				mediaInfo.clear();

				if (files.size() > FILES_FOUND_LIMIT) {
					//Emits the signal every FILES_FOUND_LIMIT files found
					emit filesFound(files);
					files.clear();
				}
			}

		}

		else {
			//Syntax error
		}
	}

	device->close();
	delete device;

	if (!files.isEmpty()) {
		//Emits the signal for the remaining files found (< FILES_FOUND_LIMIT)
		emit filesFound(files);
	}

	return true;
}

bool CUEParser::save(const QString & location, const QList<MediaInfo> & files) {
	QIODevice * device = Util::openLocationWriteMode(location);
	if (!device) {
		return false;
	}

	_stop = false;

	QString path(QFileInfo(location).path());

	QTextStream stream(device);

	stream << "REM COMMENT"
		<< CUE_QUOTE << QCoreApplication::applicationName() << CUE_QUOTE
		<< CUE_EOL;

	foreach (MediaInfo mediaInfo, files) {
		if (_stop) {
			break;
		}

		stream << "FILE "
			<< CUE_QUOTE;
		if (MediaInfo::isUrl(mediaInfo.fileName())) {
			stream << mediaInfo.fileName();
		} else {
			//Try to save the filename as relative instead of absolute
			QString filename(TkFile::relativeFilePath(path, mediaInfo.fileName()));
			stream << Util::pathToNativeSeparators(filename);
		}
		stream << CUE_QUOTE
			<< " WAVE"
			<< CUE_EOL;

		stream << "  TRACK "
			<< mediaInfo.metaDataValue(MediaInfo::TrackNumber).toInt()
			<< " AUDIO"
			<< CUE_EOL;

		stream << "    TITLE "
			<< CUE_QUOTE << mediaInfo.metaDataValue(MediaInfo::Title).toString() << CUE_QUOTE
			<< CUE_EOL;

		stream << "    PERFORMER "
			<< CUE_QUOTE << mediaInfo.metaDataValue(MediaInfo::Artist).toString() << CUE_QUOTE
			<< CUE_EOL;

		stream << "    INDEX "
			<< "01 "
			<< mediaInfo.cueStartIndexFormatted()
			<< CUE_EOL;
	}

	device->close();
	delete device;

	return true;
}
