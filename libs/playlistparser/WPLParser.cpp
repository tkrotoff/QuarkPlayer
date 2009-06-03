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

static const char * WPL_TITLE = "title";
static const char * WPL_SMIL = "smil";
static const char * WPL_HEAD = "head";
static const char * WPL_META = "meta";
static const char * WPL_NAME = "name";
static const char * WPL_GENERATOR = "generator";
static const char * WPL_CONTENT = "content";
static const char * WPL_AUTHOR = "author";
static const char * WPL_BODY = "body";
static const char * WPL_SEQ = "seq";
static const char * WPL_MEDIA = "media";
static const char * WPL_SRC = "src";

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

			switch (xml.tokenType()) {

			case QXmlStreamReader::StartElement: {
				QString element(xml.name().toString());

				if (element.compare(WPL_MEDIA, Qt::CaseInsensitive) == 0) {
					QString filename(xml.attributes().value(WPL_SRC).toString().trimmed());

					if (!filename.isEmpty()) {
						//Add file to the list of files
						files << MediaInfo(Util::canonicalFilePath(path, filename));

						if (files.size() > FILES_FOUND_LIMIT) {
							//Emits the signal every FILES_FOUND_LIMIT files found
							emit filesFound(files);
							files.clear();
						}
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

		xml.writeStartElement(WPL_SMIL);
			xml.writeStartElement(WPL_HEAD);
				xml.writeStartElement(WPL_META);
					xml.writeAttribute(WPL_NAME, WPL_GENERATOR);
					xml.writeAttribute(WPL_CONTENT, QCoreApplication::applicationName());
				xml.writeEndElement();

				xml.writeStartElement(WPL_AUTHOR);
				xml.writeEndElement();

				xml.writeTextElement(WPL_TITLE, QFileInfo(_filename).baseName());
			xml.writeEndElement();	//head

			xml.writeStartElement(WPL_BODY);
				xml.writeStartElement(WPL_SEQ);

					foreach (MediaInfo mediaInfo, files) {
						if (_stop) {
							break;
						}
						xml.writeStartElement(WPL_MEDIA);

						if (mediaInfo.isUrl()) {
							xml.writeAttribute(WPL_SRC, mediaInfo.fileName());
						} else {
							//Try to save the filename as relative instead of absolute
							QString filename(TkFile::relativeFilePath(path, mediaInfo.fileName()));
							filename = Util::pathToNativeSeparators(filename);
							xml.writeAttribute(WPL_SRC, filename);
						}

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
