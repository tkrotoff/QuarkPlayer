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

#include "ASXParser.h"

#include "Util.h"
#include "PlaylistParserLogger.h"

#include <MediaInfoFetcher/MediaInfo.h>

#include <QtXml/QXmlStreamReader>
#include <QtXml/QXmlStreamWriter>

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>

static const char * ASX_TITLE = "title";
static const char * ASX_ENTRY = "entry";
static const char * ASX_AUTHOR = "author";
static const char * ASX_MOREINFO = "moreinfo";
static const char * ASX_REF = "ref";
static const char * ASX_HREF = "href";
static const char * ASX_COPYRIGHT = "copyright";
static const char * ASX_ASX = "asx";
static const char * ASX_VERSION = "version";
static const char * ASX_3DOT0 = "3.0";

ASXParser::ASXParser(QObject * parent)
	: IPlaylistParserImpl(parent) {

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

void ASXParser::load(QIODevice * device, const QString & location) {
	_stop = false;

	QList<MediaInfo> files;

	QString path(QFileInfo(location).path());

	MediaInfo mediaInfo;

	//Some ASX playlists are invalid XML files
	//Yes ASX format is shitty
	//Let's fix this

	QString text(device->readAll());
	device->close();

	//Replace all ASX tags by lowercase ASX tags,
	//otherwise the XML parser can fail
	text.replace(ASX_TITLE, ASX_TITLE, Qt::CaseInsensitive);
	text.replace(ASX_ENTRY, ASX_ENTRY, Qt::CaseInsensitive);
	text.replace(ASX_AUTHOR, ASX_AUTHOR, Qt::CaseInsensitive);
	text.replace(ASX_MOREINFO, ASX_MOREINFO, Qt::CaseInsensitive);
	text.replace(ASX_REF, ASX_REF, Qt::CaseInsensitive);
	text.replace(ASX_HREF, ASX_HREF, Qt::CaseInsensitive);
	text.replace(ASX_COPYRIGHT, ASX_COPYRIGHT, Qt::CaseInsensitive);
	text.replace(ASX_ASX, ASX_ASX, Qt::CaseInsensitive);
	text.replace(ASX_VERSION, ASX_VERSION, Qt::CaseInsensitive);
	///

	//New lines should be are \n only
	text.replace("\r\n", "\n");

	//Replace <ref href="http://proxiregie.adswizz.com/www/delivery/radioPreRoll.php?zoneid=124&streamtype=http&cs=330384"/>
	//By <ref href="http://proxiregie.adswizz.com/www/delivery/radioPreRoll.php?zoneid=124&amp;streamtype=http&amp;cs=330384"/>
	QStringList lines = text.split('\n', QString::SkipEmptyParts);
	QString newText;
	foreach (QString line, lines) {
		QRegExp quotes("\"([^\"]*)\"");
		if (quotes.indexIn(line) > -1) {
			QString tmp = quotes.cap(1);
			//Replace all occurrences of & with &amp;
			//See http://discuss.joyent.com/viewtopic.php?id=10661
			QRegExp rx_ampersand("&(?![a-zA-Z]{1,8};)");
			tmp.replace(rx_ampersand, "&amp;");
			if (!tmp.isEmpty()) {
				line.replace(quotes, "\"" + tmp + "\"");
			}
		}
		newText += line;
	}
	///

	QXmlStreamReader xml(newText);
	while (!xml.atEnd() && !_stop) {
		xml.readNext();

		switch (xml.tokenType()) {

		case QXmlStreamReader::StartElement: {
			QString element(xml.name().toString());
			if (element.compare(ASX_TITLE, Qt::CaseInsensitive) == 0) {
				QString title(xml.readElementText());
				mediaInfo.setMetaData(MediaInfo::Title, title);
			} else if (element.compare(ASX_REF, Qt::CaseInsensitive) == 0) {
				QString url(xml.attributes().value(ASX_HREF).toString());
				if (url.isEmpty()) {
					//Yes ASX format is shit
					//Let's try with uppercase
					QString uppercase(ASX_HREF);
					uppercase = uppercase.toUpper();
					url = xml.attributes().value(uppercase).toString();
				}
				if (mediaInfo.fileName().isEmpty()) {
					//Do not set again a URL if already set
					//Windows Media Player 12 under Windows 7
					//Example of a wrong ASX playlist:
					//<ASX VERSION="3.0">
					//<ABSTRACT>Virgin Radio</ABSTRACT>
					//<TITLE>Virgin Radio</TITLE>
					//<AUTHOR>Virgin Radio - YACAST FRANCE</AUTHOR>
					//<COPYRIGHT>(c) 2008, Virgin Radio</COPYRIGHT>
					//<ENTRY>
					//	<REF HREF="mms://viplagardere.yacast.net/encodereurope2" />
					//	<REF HREF="mms://vipmms9.yacast.net/encodereurope2" />
					//</ENTRY>
					//</ASX>
					mediaInfo.setFileName(url);
				}
			} else if (element.compare(ASX_COPYRIGHT, Qt::CaseInsensitive) == 0) {
				QString copyright(xml.readElementText());
				mediaInfo.setMetaData(MediaInfo::Copyright, copyright);
			}
			break;
		}

		case QXmlStreamReader::EndElement: {
			QString element(xml.name().toString());
			if (element.compare(ASX_ENTRY, Qt::CaseInsensitive) == 0) {
				if (!mediaInfo.fileName().isEmpty()) {
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
			}
			break;
		}

		default:
			//Do nothing
			break;

		}
	}

	if (xml.hasError()) {
		PlaylistParserWarning() << "Error:"
			<< xml.errorString()
			<< "line:" << xml.lineNumber()
			<< "column:" << xml.columnNumber();
	}

	if (!files.isEmpty()) {
		//Emits the signal for the remaining files found (< FILES_FOUND_LIMIT)
		emit filesFound(files);
	}
}

void ASXParser::save(QIODevice * device, const QString & location, const QList<MediaInfo> & files) {
	_stop = false;

	QXmlStreamWriter xml(device);
	xml.setAutoFormatting(true);
	//Don't write <?xml version="1.0" encoding="UTF-8"?>
	//ASX playlist format is not a real XML file!
	//xml.writeStartDocument();

	xml.writeStartElement(ASX_ASX);
	xml.writeAttribute(ASX_VERSION, ASX_3DOT0);
		xml.writeTextElement(ASX_TITLE, QFileInfo(location).completeBaseName());

		foreach (MediaInfo mediaInfo, files) {
			if (_stop) {
				break;
			}

			xml.writeStartElement(ASX_ENTRY);
				QString title(mediaInfo.metaDataValue(MediaInfo::Title).toString());
				if (!title.isEmpty()) {
					xml.writeTextElement(ASX_TITLE, title);
				}

				xml.writeStartElement(ASX_REF);
					QString filename(mediaInfo.fileName());
					xml.writeAttribute(ASX_HREF, filename);
				xml.writeEndElement();	//ref

				QString copyright(mediaInfo.metaDataValue(MediaInfo::Copyright).toString());
				if (!copyright.isEmpty()) {
					xml.writeTextElement(ASX_COPYRIGHT, copyright);
				}
			xml.writeEndElement();	//entry
		}
	xml.writeEndElement();	//asx

	device->close();
}
