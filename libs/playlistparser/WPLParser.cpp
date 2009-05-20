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

#include "WPLParser.h"

#include "Util.h"

#include <mediainfowindow/MediaInfo.h>

#include <QtXml/QXmlStreamReader>
#include <QtXml/QXmlStreamWriter>

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTime>
#include <QtCore/QDebug>

WPLParser::WPLParser(const QString & filename, QObject * parent)
	: IPlaylistParser(filename, parent) {

	_filename = filename;
	_stop = false;
}

WPLParser::~WPLParser() {
	stop();
}

QStringList WPLParser::fileExtensions() const {
	QStringList extensions;
	extensions << "wpl";
	return extensions;
}

void WPLParser::stop() {
	_stop = true;
}

void WPLParser::load() {
	QTime timeElapsed;
	timeElapsed.start();

	_stop = false;

	QList<MediaInfo> files;

	qDebug() << __FUNCTION__ << "Playlist:" << _filename;

	QFile file(_filename);
	if (file.open(QIODevice::ReadOnly)) {
		QString path(QFileInfo(_filename).path());

		QXmlStreamReader xml(&file);
		while (!xml.atEnd() && !_stop) {
			xml.readNext();

			if (xml.isStartElement()) {
				if (xml.name() == "media") {
					QString filename(xml.attributes().value("src").toString());

					//Add file to the list of files
					files << MediaInfo(Util::canonicalFilePath(path, filename));

					if (files.size() > FILES_FOUND_LIMIT) {
						//Emits the signal every FILES_FOUND_LIMIT files found
						emit filesFound(files);
						files.clear();
					}
				}
			}
		}

		if (xml.hasError()) {
			qCritical() << __FUNCTION__ << "Error: ";
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

void WPLParser::save(const QList<MediaInfo> & files) {
	QTime timeElapsed;
	timeElapsed.start();

	_stop = false;

	qDebug() << __FUNCTION__ << "Playlist:" << _filename;

	QString path(QFileInfo(_filename).path());

	QFile file(_filename);
	if (file.open(QIODevice::WriteOnly)) {

		QXmlStreamWriter xml(&file);
		xml.setAutoFormatting(true);
		xml.writeStartDocument();

		xml.writeStartElement("smil");
			xml.writeStartElement("head");
				xml.writeStartElement("meta");
					xml.writeAttribute("name", "Generator");
					xml.writeAttribute("content", QCoreApplication::applicationName());
				xml.writeEndElement();

				xml.writeStartElement("author");
				xml.writeEndElement();

				xml.writeTextElement("title", QFileInfo(_filename).baseName());
			xml.writeEndElement();	//head

			xml.writeStartElement("body");
				xml.writeStartElement("seq");

					foreach (MediaInfo mediaInfo, files) {
						if (_stop) {
							break;
						}
						xml.writeStartElement("media");

						//Try to save the filename as relative instead of absolute
						QString filename(Util::relativeFilePath(path, mediaInfo.fileName()));
						filename = Util::pathToNativeSeparators(filename);
						xml.writeAttribute("src", filename);

						xml.writeEndElement();	//media
					}

				xml.writeEndElement();	//seq
			xml.writeEndElement();	//body
		xml.writeEndElement();	//smil
	}

	file.close();

	//Emits the last signal
	emit finished(timeElapsed.elapsed());
}
