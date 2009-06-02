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

#include "ASXParser.h"

#include "Util.h"

#include <mediainfowindow/MediaInfo.h>
#include <tkutil/TkFile.h>

#include <QtXml/QXmlStreamReader>
#include <QtXml/QXmlStreamWriter>

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTime>
#include <QtCore/QDebug>

ASXParser::ASXParser(const QString & filename, QObject * parent)
	: IPlaylistParser(filename, parent) {

	_filename = filename;
	_stop = false;
}

ASXParser::~ASXParser() {
	stop();
}

QStringList ASXParser::fileExtensions() const {
	QStringList extensions;
	extensions << "asx";
	extensions << "wax";
	extensions << "wvx";
	return extensions;
}

void ASXParser::stop() {
	_stop = true;
}

void ASXParser::load() {
	QTime timeElapsed;
	timeElapsed.start();

	_stop = false;

	QList<MediaInfo> files;

	qDebug() << __FUNCTION__ << "Playlist:" << _filename;

	QFile file(_filename);
	if (file.open(QIODevice::ReadOnly)) {
		QString path(QFileInfo(_filename).path());

		MediaInfo mediaInfo;

		QXmlStreamReader xml(&file);
		while (!xml.atEnd() && !_stop) {
			xml.readNext();

			switch (xml.tokenType()) {
			case QXmlStreamReader::StartElement: {
				QString element(xml.name().toString());
				if (element.compare("title", Qt::CaseInsensitive) == 0) {
					QString title(xml.readElementText().trimmed());
					if (!title.isEmpty()) {
						mediaInfo.insertMetadata(MediaInfo::Title, title);
					}
				} else if (element.compare("ref", Qt::CaseInsensitive) == 0) {
					QString url(xml.attributes().value("href").toString().trimmed());
					if (!url.isEmpty()) {
						mediaInfo.setFileName(url);
						mediaInfo.setUrl(true);
					}
				} else if (element.compare("copyright", Qt::CaseInsensitive) == 0) {
					QString copyright(xml.readElementText().trimmed());
					if (!copyright.isEmpty()) {
						mediaInfo.insertMetadata(MediaInfo::Copyright, copyright);
					}
				}
				}
				break;
			case QXmlStreamReader::EndElement: {
				QString element(xml.name().toString());
				if (element.compare("entry", Qt::CaseInsensitive) == 0) {
					//Add file to the list of files
					files << mediaInfo;

					//Clear the MediaInfo
					mediaInfo.clear();

					if (files.size() > FILES_FOUND_LIMIT) {
						//Emits the signal every FILES_FOUND_LIMIT files found
						emit filesFound(files);
						files.clear();
					}
				}
				break;
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

void ASXParser::save(const QList<MediaInfo> & files) {
	QTime timeElapsed;
	timeElapsed.start();

	_stop = false;

	qDebug() << __FUNCTION__ << "Playlist:" << _filename;

	QString path(QFileInfo(_filename).path());

	QFile file(_filename);
	if (file.open(QIODevice::WriteOnly)) {

		QXmlStreamWriter xml(&file);
		xml.setAutoFormatting(true);

		xml.writeStartElement("asx");
		xml.writeAttribute("version", "3.0");
			xml.writeTextElement("title", QFileInfo(_filename).baseName());

			foreach (MediaInfo mediaInfo, files) {
				if (_stop) {
					break;
				}

				xml.writeStartElement("entry");
					QString title(mediaInfo.metadataValue(MediaInfo::Title));
					if (!title.isEmpty()) {
						xml.writeTextElement("title", title);
					}

					xml.writeStartElement("ref");
						QString filename(mediaInfo.fileName());
						xml.writeAttribute("href", filename);
					xml.writeEndElement();	//ref

					QString copyright(mediaInfo.metadataValue(MediaInfo::Copyright));
					if (!copyright.isEmpty()) {
						xml.writeTextElement("copyright", copyright);
					}
				xml.writeEndElement();	//entry
			}
		xml.writeEndElement();	//asx
	}

	file.close();

	//Emits the last signal
	emit finished(timeElapsed.elapsed());
}
