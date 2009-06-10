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

#include "PLSParser.h"

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

static const char * PLS_PLAYLIST = "[playlist]";
static const char * PLS_FILE = "File";
static const char * PLS_TITLE = "Title";
static const char * PLS_LENGTH = "Length";

//Winamp inserts some ugly CR character inside .pls files
//To be compatible with Winamp, each end of line should be "\r\n"
//Tested with Winamp 5.531 (april 2008)
static const char * PLS_WINAMP_EOL = "\r\r\n";
static const char * PLS_EOL = "\r";

PLSParser::PLSParser(const QString & filename, QObject * parent)
	: IPlaylistParser(filename, parent) {

	_filename = filename;
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

void PLSParser::load() {
	QTime timeElapsed;
	timeElapsed.start();

	_stop = false;

	QList<MediaInfo> files;

	qDebug() << __FUNCTION__ << "Playlist:" << _filename;

	//See http://regexlib.com/DisplayPatterns.aspx

	//File1=http://streamexample.com:80
	static QRegExp rx_file("^File(\\d+)=(.*)$");
	//Title1=My Favorite Online Radio
	static QRegExp rx_title("^Title(\\d+)=(.*)$");
	//Length1=-1
	static QRegExp rx_length("^Length(\\d+)=(.*)$");

	MediaInfo mediaInfo;

	QFile file(_filename);
	if (file.open(QIODevice::ReadOnly)) {
		QString path(QFileInfo(_filename).path());

		QTextStream stream(&file);

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
				mediaInfo.setUrl(isUrl);
				if (isUrl) {
					mediaInfo.setFileName(filename);
				} else {
					mediaInfo.setFileName(Util::canonicalFilePath(path, filename));
				}
			}

			else if (rx_title.indexIn(line) != -1) {
				QString title(rx_title.cap(2));
				mediaInfo.insertMetadata(MediaInfo::Title, title);
			}

			else if (rx_length.indexIn(line) != -1) {
				QString length(rx_length.cap(2));
				mediaInfo.setLength(length.toInt());
			}

			else {
				//Syntax error
			}
		}
	}

	file.close();

	if (!mediaInfo.fileName().isEmpty()) {
		//Add the last file to the list of files
		files << mediaInfo;
	}

	if (!files.isEmpty()) {
		//Emits the signal for the remaining files found (< FILES_FOUND_LIMIT)
		emit filesFound(files);
	}

	//Emits the last signal
	emit finished(timeElapsed.elapsed());
}

void PLSParser::save(const QList<MediaInfo> & files) {
	QTime timeElapsed;
	timeElapsed.start();

	_stop = false;

	qDebug() << __FUNCTION__ << "Playlist:" << _filename;

	QString path(QFileInfo(_filename).path());

	QFile file(_filename);
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream stream(&file);

		QString filename;

		stream << PLS_PLAYLIST << PLS_WINAMP_EOL;

		int count = 0;
		foreach (MediaInfo mediaInfo, files) {
			count++;

			if (_stop) {
				break;
			}

			stream << PLS_FILE << count << '=';
			if (mediaInfo.isUrl()) {
				stream << mediaInfo.fileName();
			} else {
				//Try to save the filename as relative instead of absolute
				QString filename(TkFile::relativeFilePath(path, mediaInfo.fileName()));
				stream << Util::pathToNativeSeparators(filename);
			}
			stream << PLS_WINAMP_EOL;

			stream << PLS_TITLE << count << '=';
			QString artist(mediaInfo.metadataValue(MediaInfo::Artist));
			if (!artist.isEmpty()) {
				stream << artist;
			}
			QString title(mediaInfo.metadataValue(MediaInfo::Title));
			if (!title.isEmpty()) {
				if (!artist.isEmpty()) {
					stream << " - ";
				}
				stream << mediaInfo.metadataValue(MediaInfo::Title);
			}
			stream << PLS_WINAMP_EOL;

			stream << PLS_LENGTH << count << '=';
			stream << mediaInfo.lengthSeconds();
			stream << PLS_WINAMP_EOL;
		}
	}

	file.close();

	//Emits the last signal
	emit finished(timeElapsed.elapsed());
}
