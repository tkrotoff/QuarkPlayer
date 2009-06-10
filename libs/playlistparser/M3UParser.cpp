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

#include "M3UParser.h"

#include "Util.h"

#include <mediainfowindow/MediaInfo.h>
#include <tkutil/TkFile.h>

#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>
#include <QtCore/QDir>
#include <QtCore/QTextCodec>
#include <QtCore/QTime>
#include <QtCore/QDebug>

static const char * M3U_EOL = "\n";

M3UParser::M3UParser(const QString & filename, QObject * parent)
	: IPlaylistParser(filename, parent) {

	_filename = filename;
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

void M3UParser::load() {
	QTime timeElapsed;
	timeElapsed.start();

	_stop = false;

	QList<MediaInfo> files;

	qDebug() << __FUNCTION__ << "Playlist:" << _filename;

	//See http://regexlib.com/DisplayPatterns.aspx

	//#EXTM3U
	static QRegExp rx_extm3u("^#EXTM3U$|^#M3U$");
	//#EXTINF:123,Sample title
	static QRegExp rx_extinf("^#EXTINF:([-+]?\\d+),(.*)$");
	//#EXTINF:Sample title
	static QRegExp rx_extinf_title("^#EXTINF:(.*)$");
	//#Just a comment
	static QRegExp rx_comment("^#.*$");

	QFile file(_filename);
	if (file.open(QIODevice::ReadOnly)) {
		QString path(QFileInfo(_filename).path());

		QTextStream stream(&file);

		if (isUtf8()) {
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
				QString length(rx_extinf.cap(1));
				if (!length.isEmpty()) {
					mediaInfo.setLength(length.toInt());
				}
				QString title(rx_extinf.cap(2));
				if (!title.isEmpty()) {
					mediaInfo.insertMetadata(MediaInfo::Title, title);
				}
			}

			else if (rx_extinf_title.indexIn(line) != -1) {
				QString title(rx_extinf_title.cap(1));
				if (!title.isEmpty()) {
					mediaInfo.insertMetadata(MediaInfo::Title, title);
				}
			}

			else if (rx_comment.indexIn(line) != -1) {
				//# line, comment, ignored
			}

			else {
				bool isUrl = MediaInfo::isUrl(line);
				mediaInfo.setUrl(isUrl);
				if (isUrl) {
					mediaInfo.setFileName(line);
				} else {
					mediaInfo.setFileName(Util::canonicalFilePath(path, line));
				}

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
	}

	file.close();

	if (!files.isEmpty()) {
		//Emits the signal for the remaining files found (< FILES_FOUND_LIMIT)
		emit filesFound(files);
	}

	//Emits the last signal
	emit finished(timeElapsed.elapsed());
}

void M3UParser::save(const QList<MediaInfo> & files) {
	QTime timeElapsed;
	timeElapsed.start();

	_stop = false;

	qDebug() << __FUNCTION__ << "Playlist:" << _filename;

	QString path(QFileInfo(_filename).path());

	QFile file(_filename);
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream stream(&file);

		if (isUtf8()) {
			stream.setCodec("UTF-8");
		} else {
			stream.setCodec(QTextCodec::codecForLocale());
		}

		QString filename;

		if (!files.isEmpty()) {
			stream << "#EXTM3U\n";
		}

		foreach (MediaInfo mediaInfo, files) {
			if (_stop) {
				break;
			}

			stream << "#EXTINF:";
			stream << mediaInfo.lengthSeconds();
			stream << ',';

			QString artist(mediaInfo.metadataValue(MediaInfo::Artist));
			if (!artist.isEmpty()) {
				stream << artist;
			}
			QString title(mediaInfo.metadataValue(MediaInfo::Title));
			if (!title.isEmpty()) {
				if (!artist.isEmpty()) {
					stream << " - ";
				}
				stream << title;
			}
			stream << M3U_EOL;

			if (mediaInfo.isUrl()) {
				stream << mediaInfo.fileName();
			} else {
				//Try to save the filename as relative instead of absolute
				QString filename(TkFile::relativeFilePath(path, mediaInfo.fileName()));
				stream << Util::pathToNativeSeparators(filename);
			}
			stream << M3U_EOL;
		}
	}

	file.close();

	//Emits the last signal
	emit finished(timeElapsed.elapsed());
}

bool M3UParser::isUtf8() const {
	return (QFileInfo(_filename).suffix().toLower() == "m3u8");
}
