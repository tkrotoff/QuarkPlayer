/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>
#include <QtCore/QDir>
#include <QtCore/QTextCodec>
#include <QtCore/QTime>
#include <QtCore/QDebug>

static const int FILES_FOUND_LIMIT = 1000;

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

	QStringList files;

	qDebug() << __FUNCTION__ << "Playlist:" << _filename;

	QRegExp extm3u("^#EXTM3U|^#M3U");
	QRegExp extinf("^#EXTINF:(.*),(.*)");

	QFile file(_filename);
	if (file.open(QIODevice::ReadOnly)) {
		QString path = QFileInfo(_filename).path();

		QTextStream stream(&file);

		if (isUtf8()) {
			stream.setCodec("UTF-8");
		} else {
			stream.setCodec(QTextCodec::codecForLocale());
		}

		while (!stream.atEnd() && !_stop) {
			//qDebug() << __FUNCTION__ << "Parsing...";

			//Line of text excluding '\n'
			QString line = stream.readLine();

			//qDebug() << __FUNCTION__ << "Line:" << line;

			if (extm3u.indexIn(line) != -1) {
				//#EXTM3U line, ignored
			}

			else if (extinf.indexIn(line) != -1) {
				//#EXTINF line
				//Do nothing with these informations
				double duration = extinf.cap(1).toDouble();
				QString name = extinf.cap(2);
				//qDebug() << __FUNCTION__ << "Name:" << name << "duration:" << duration;
			}

			else if (line.startsWith("#")) {
				//# line, comment, ignored
			}

			else {
				QString filename = line;
				QFileInfo fileInfo(filename);
				if (fileInfo.exists()) {
					filename = fileInfo.absoluteFilePath();
				}
				if (!fileInfo.exists()) {
					if (QFileInfo(path + "/" + filename).exists()) {
						filename = path + "/" + filename;
					}
				}

				//Add file to the list of files
				files << filename;

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

void M3UParser::save(const QStringList & files) {
	QTime timeElapsed;
	timeElapsed.start();

	_stop = false;

	qDebug() << __FUNCTION__ << "Playlist:" << _filename;

	QString path = QFileInfo(_filename).path();
	if (!path.endsWith("/")) {
		path += "/";
	}

#ifdef Q_OS_WIN
	path = changeSlashes(path);
#endif

	qDebug() << __FUNCTION__ << "Dir path:" << path;

	QFile file(_filename);
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream stream(&file);

		if (isUtf8()) {
			stream.setCodec("UTF-8");
		} else {
			stream.setCodec(QTextCodec::codecForLocale());
		}

		QString filename;

		stream << "#EXTM3U" << "\n";

		foreach (QString filename, files) {
			if (_stop) {
				break;
			}

			/*
			stream << "#EXTINF:";
			stream << duration << ",";
			stream << name << "\n";
			*/

#ifdef Q_OS_WIN
			filename = changeSlashes(filename);
#endif

			//Try to save the filename as relative instead of absolute
			if (filename.startsWith(path)) {
				filename = filename.mid(path.length());
			}
			stream << filename << "\n";
		}

		file.close();
	}

	//Emits the last signal
	emit finished(timeElapsed.elapsed());
}

QString M3UParser::changeSlashes(const QString & filename) {
	QString tmp(filename);

	//Only change if file exists (it's a local file)
	if (QFileInfo(tmp).exists()) {
		tmp = QDir::toNativeSeparators(tmp);
	}

	return tmp;
}

bool M3UParser::isUtf8() const {
	return QFileInfo(_filename).suffix().toLower() == "m3u8";
}
