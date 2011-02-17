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

#include "M3UParser.h"

#include "Util.h"
#include "PlaylistParserLogger.h"

#include <MediaInfoFetcher/MediaInfo.h>
#include <TkUtil/TkFile.h>

#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>
#include <QtCore/QTextCodec>

static const char * M3U_EOL = "\n";

M3UParser::M3UParser(QObject * parent)
	: IPlaylistParserImpl(parent) {

	_stop = false;
}

M3UParser::~M3UParser() {
	stop();
}

QStringList M3UParser::fileExtensions() const {
	QStringList extensions;
	extensions << "m3u";
	extensions << "m3u8";
	return extensions;
}

void M3UParser::stop() {
	_stop = true;
}

bool M3UParser::load(const QString & location) {
	QIODevice * device = Util::openLocationReadMode(location);
	if (!device) {
		return false;
	}

	_stop = false;

	QList<MediaInfo> files;

	//See http://regexlib.com/DisplayPatterns.aspx

	//#EXTM3U
	QRegExp rx_extm3u("^#EXTM3U$|^#M3U$");
	//#EXTINF:123,Sample title
	//"Sample title" can be "Artist - Title"
	QRegExp rx_extinf("^#EXTINF:([-+]?\\d+),\\s?(.*)$");
	//#EXTINF:Sample title
	QRegExp rx_extinf_title("^#EXTINF:\\s?(.*)$");
	//#Just a comment
	QRegExp rx_comment("^#.*$");

	QString path(QFileInfo(location).path());

	QTextStream stream(device);

	if (isUtf8(location)) {
		stream.setCodec("UTF-8");
	} else {
		stream.setCodec(QTextCodec::codecForLocale());
	}

	MediaInfo mediaInfo;

	while (!stream.atEnd() && !_stop) {
		//Line of text excluding '\n'
		QString line(stream.readLine().trimmed());

		if (line.isEmpty()) {
			//Do nothing
		}

		else if (rx_extm3u.indexIn(line) != -1) {
			//#EXTM3U line, ignored
		}

		else if (rx_extinf.indexIn(line) != -1) {
			//#EXTINF line
			QString duration(rx_extinf.cap(1));
			if (!duration.isEmpty()) {
				mediaInfo.setDurationSecs(duration.toInt());
			}
			QString title(rx_extinf.cap(2));
			if (!title.isEmpty()) {
				mediaInfo.setMetaData(MediaInfo::Title, title);
			}
		}

		else if (rx_extinf_title.indexIn(line) != -1) {
			QString title(rx_extinf_title.cap(1));
			if (!title.isEmpty()) {
				mediaInfo.setMetaData(MediaInfo::Title, title);
			}
		}

		else if (rx_comment.indexIn(line) != -1) {
			//# line, comment, ignored
		}

		else {
			bool isUrl = MediaInfo::isUrl(line);
			if (isUrl) {
				mediaInfo.setFileName(line);
			} else {
				mediaInfo.setFileName(Util::canonicalFilePath(path, line));
			}
			PlaylistParserDebug() << isUrl << path << line;

			//Add file to the list of files
			files << mediaInfo;

			//Clear the MediaInfo for the next 2 lines from the m3u playlist
			mediaInfo.clear();

			if (files.size() > FILES_FOUND_LIMIT) {
				//Emits the signal every FILES_FOUND_LIMIT files found
				emit filesFound(files);
				files.clear();
			}
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

bool M3UParser::save(const QString & location, const QList<MediaInfo> & files) {
	QIODevice * device = Util::openLocationWriteMode(location);
	if (!device) {
		return false;
	}

	_stop = false;

	QString path(QFileInfo(location).path());

	QTextStream stream(device);

	if (isUtf8(location)) {
		stream.setCodec("UTF-8");
	} else {
		stream.setCodec(QTextCodec::codecForLocale());
	}

	if (!files.isEmpty()) {
		stream << "#EXTM3U\n";
	}

	foreach (MediaInfo mediaInfo, files) {
		if (_stop) {
			break;
		}

		stream << "#EXTINF:";
		stream << mediaInfo.durationSecs();
		stream << ',';

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
		stream << M3U_EOL;

		if (MediaInfo::isUrl(mediaInfo.fileName())) {
			stream << mediaInfo.fileName();
		} else {
			//Try to save the filename as relative instead of absolute
			QString filename(TkFile::relativeFilePath(path, mediaInfo.fileName()));
			stream << Util::pathToNativeSeparators(filename);
		}
		stream << M3U_EOL;
	}

	device->close();
	delete device;

	return true;
}

bool M3UParser::isUtf8(const QString & location) {
	return (QFileInfo(location).suffix().toLower() == "m3u8");
}
