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

#include "PLSParser.h"

#include "Util.h"
#include "PlaylistParserLogger.h"

#include <MediaInfoFetcher/MediaInfo.h>
#include <TkUtil/TkFile.h>

#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>
#include <QtCore/QTextCodec>

static const char * PLS_PLAYLIST = "[playlist]";
static const char * PLS_FILE = "File";
static const char * PLS_TITLE = "Title";
static const char * PLS_LENGTH = "Length";

//Winamp inserts some ugly CR character inside .pls files
//To be compatible with Winamp, each end of line should be "\r\n"
//Tested with Winamp 5.531 (april 2008)
static const char * PLS_WINAMP_EOL = "\r\r\n";
static const char * PLS_EOL = "\r";

PLSParser::PLSParser(QObject * parent)
	: IPlaylistParserImpl(parent) {

	_stop = false;
}

PLSParser::~PLSParser() {
	stop();
}

QStringList PLSParser::fileExtensions() const {
	QStringList extensions;
	extensions << "pls";
	return extensions;
}

void PLSParser::stop() {
	_stop = true;
}

bool PLSParser::load(const QString & location) {
	QIODevice * device = Util::openLocationReadMode(location);
	if (!device) {
		return false;
	}

	_stop = false;

	QList<MediaInfo> files;

	//See http://regexlib.com/DisplayPatterns.aspx

	//File1=http://streamexample.com:80
	QRegExp rx_file("^File(\\d+)=(.*)$");
	//Title1=My Favorite Online Radio
	QRegExp rx_title("^Title(\\d+)=(.*)$");
	//Length1=-1
	QRegExp rx_length("^Length(\\d+)=(.*)$");

	MediaInfo mediaInfo;

	QString path(QFileInfo(location).path());

	QTextStream stream(device);

	while (!stream.atEnd() && !_stop) {

		//Line of text excluding '\n'
		QString line(stream.readLine().trimmed());

		//Winamp inserts some ugly CR character inside .pls files
		//Let's ignore them
		line.remove(PLS_EOL);

		if (rx_file.indexIn(line) != -1) {
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

			QString filename(rx_file.cap(2));
			bool isUrl = MediaInfo::isUrl(filename);
			if (isUrl) {
				mediaInfo.setFileName(filename);
			} else {
				mediaInfo.setFileName(Util::canonicalFilePath(path, filename));
			}
		}

		else if (rx_title.indexIn(line) != -1) {
			QString title(rx_title.cap(2));
			mediaInfo.setMetaData(MediaInfo::Title, title);
		}

		else if (rx_length.indexIn(line) != -1) {
			QString length(rx_length.cap(2));
			mediaInfo.setDurationSecs(length.toInt());
		}

		else if (line == "[playlist]") {
			//Ignore
		}

		else if (line.startsWith("NumberOfEntries=")) {
			//Ignore
		}

		else if (line.startsWith("Version=")) {
			//Ignore
		}

		else {
			PlaylistParserWarning() << "Syntax error:" << line;
		}
	}

	device->close();
	delete device;

	if (!mediaInfo.fileName().isEmpty()) {
		//Add the last file to the list of files
		files << mediaInfo;
	}

	if (!files.isEmpty()) {
		//Emits the signal for the remaining files found (< FILES_FOUND_LIMIT)
		emit filesFound(files);
	}

	return true;
}

bool PLSParser::save(const QString & location, const QList<MediaInfo> & files) {
	QIODevice * device = Util::openLocationWriteMode(location);
	if (!device) {
		return false;
	}

	_stop = false;

	QString path(QFileInfo(location).path());

	QTextStream stream(device);

	stream << PLS_PLAYLIST << PLS_WINAMP_EOL;

	int count = 0;
	foreach (MediaInfo mediaInfo, files) {
		count++;

		if (_stop) {
			break;
		}

		stream << PLS_FILE << count << '=';
		if (MediaInfo::isUrl(mediaInfo.fileName())) {
			stream << mediaInfo.fileName();
		} else {
			//Try to save the filename as relative instead of absolute
			QString filename(TkFile::relativeFilePath(path, mediaInfo.fileName()));
			stream << Util::pathToNativeSeparators(filename);
		}
		stream << PLS_WINAMP_EOL;

		stream << PLS_TITLE << count << '=';
		QString artist(mediaInfo.metaDataValue(MediaInfo::Artist).toString());
		if (!artist.isEmpty()) {
			stream << artist;
		}
		QString title(mediaInfo.metaDataValue(MediaInfo::Title).toString());
		if (!title.isEmpty()) {
			if (!artist.isEmpty()) {
				stream << " - ";
			}
			stream << title;
		}
		stream << PLS_WINAMP_EOL;

		stream << PLS_LENGTH << count << '=';
		stream << mediaInfo.durationSecs();
		stream << PLS_WINAMP_EOL;
	}

	device->close();
	delete device;

	return true;
}
