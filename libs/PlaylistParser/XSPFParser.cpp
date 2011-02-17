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

#include "XSPFParser.h"

#include "Util.h"
#include "PlaylistParserLogger.h"

#include <MediaInfoFetcher/MediaInfo.h>

#include <QtXml/QXmlStreamReader>
#include <QtXml/QXmlStreamWriter>

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QDateTime>

static const char * XSPF_BASE = "xml:base";
static const char * XSPF_DATE = "date";
static const char * XSPF_PLAYLIST = "playlist";
static const char * XSPF_TRACK = "track";
static const char * XSPF_TRACKLIST = "trackList";
static const char * XSPF_LOCATION = "location";
static const char * XSPF_IDENTIFIER = "identifier";
static const char * XSPF_CREATOR = "creator";
static const char * XSPF_ALBUM = "album";
static const char * XSPF_TRACKNUM = "trackNum";
static const char * XSPF_TITLE = "title";
static const char * XSPF_ANNOTATION = "annotation";
static const char * XSPF_DURATION = "duration";
static const char * XSPF_IMAGE = "image";
static const char * XSPF_INFO = "info";
static const char * XSPF_META = "meta";
static const char * XSPF_APPLICATION = "application";
static const char * XSPF_EXTENSION = "extension";

//Specific to foobar2000 XSPF plugin
static const char * XSPF_FOOBAR2000_DATE = "date";
static const char * XSPF_FOOBAR2000_GENRE = "genre";
///

//Specific to QuarkPlayer
static const char * XSPF_QUARKPLAYER_NAMESPACE = "http://quarkplayer.googlecode.com";
static const char * XSPF_QUARKPLAYER_CUE_START_INDEX = "cueStartIndex";
static const char * XSPF_QUARKPLAYER_CUE_END_INDEX = "cueEndIndex";
static const char * XSPF_QUARKPLAYER_YEAR = "year";
static const char * XSPF_QUARKPLAYER_GENRE = "genre";
///

XSPFParser::XSPFParser(QObject * parent)
	: IPlaylistParserImpl(parent) {

	_stop = false;
}

XSPFParser::~XSPFParser() {
	stop();
}

QStringList XSPFParser::fileExtensions() const {
	QStringList extensions;
	extensions << "xspf";
	return extensions;
}

void XSPFParser::stop() {
	_stop = true;
}

void XSPFParser::readTrack(QXmlStreamReader & xml, MediaInfo & mediaInfo, const QString & base) const {
	while (!xml.atEnd() && !_stop) {
		xml.readNext();

		QString element(xml.name().toString());

		if (xml.isStartElement()) {

			//Filename
			if (element == XSPF_LOCATION) {
				QString originalLocation = base + xml.readElementText();

				//Workaround for a bug with foorbar2000 XSPF plugin
				//"C:/1.mp3" should be written "file:///C:/1.mp3" instead of
				//"file://C:/1.mp3"
				bool match = QRegExp("^file://[A-Za-z_]:/.*$").exactMatch(originalLocation);
				if (match) {
					originalLocation = originalLocation.replace("file://", "file:///");
				}
				///

				QUrl url = QUrl::fromEncoded(originalLocation.toUtf8());
				QString location(url.toString());

				if (MediaInfo::isUrl(location)) {
					mediaInfo.setFileName(location);
				} else {
					QString path(QFileInfo(location).path());
					mediaInfo.setFileName(Util::canonicalFilePath(path, location));
				}
			}

			//Unique ID
			else if (element == XSPF_IDENTIFIER) {
				QString identifier(xml.readElementText());
				//FIXME not implemented yet
			}

			//Artist
			else if (element == XSPF_CREATOR) {
				QString creator(xml.readElementText());
				mediaInfo.setMetaData(MediaInfo::Artist, creator);
			}

			//Album
			else if (element == XSPF_ALBUM) {
				QString album(xml.readElementText());
				mediaInfo.setMetaData(MediaInfo::Album, album);
			}

			//Track number
			else if (element == XSPF_TRACKNUM) {
				QString trackNum(xml.readElementText());
				mediaInfo.setMetaData(MediaInfo::TrackNumber, trackNum.toInt());
			}

			//Title
			else if (element == XSPF_TITLE) {
				QString title(xml.readElementText());
				mediaInfo.setMetaData(MediaInfo::Title, title);
			}

			//Comment
			else if (element == XSPF_ANNOTATION) {
				QString annotation(xml.readElementText());
				if (mediaInfo.metaDataValue(MediaInfo::Title).toString().isEmpty()) {
					//Some people didn't understand how XSPF works
					//and confused annotation with title
					mediaInfo.setMetaData(MediaInfo::Title, annotation);
				}
				mediaInfo.setMetaData(MediaInfo::Comment, annotation);
			}

			//Duration/length
			else if (element == XSPF_DURATION) {
				int duration = xml.readElementText().toInt();
				//XSPF gives us the duration in milliseconds
				mediaInfo.setDurationMSecs(duration);
			}

			//Album art URL
			else if (element == XSPF_IMAGE) {
				QString image(xml.readElementText());
				//FIXME not implemented yet
				//mediaInfo.setMetaData(MediaInfo::AlbumArt, image);
			}

			//URL of the original web page
			else if (element == XSPF_INFO) {
				QString info(xml.readElementText());
				mediaInfo.setMetaData(MediaInfo::URL, QUrl(info));
			}

			//Meta
			else if (element == XSPF_META) {

				//These tags are specific to foobar2000 XSPF plugin

				QXmlStreamAttributes attributes = xml.attributes();

				//Date
				if (attributes.hasAttribute(XSPF_FOOBAR2000_DATE)) {
					QString date(attributes.value(XSPF_FOOBAR2000_DATE).toString());
					mediaInfo.setMetaData(MediaInfo::Year, QDate::fromString(date));
				}

				//Genre
				else if (attributes.hasAttribute(XSPF_FOOBAR2000_GENRE)) {
					QString genre(attributes.value(XSPF_FOOBAR2000_GENRE).toString());
					mediaInfo.setMetaData(MediaInfo::Genre, genre);
				}
			}

			else if (element == XSPF_EXTENSION) {
				QString xspfNamespace(xml.attributes().value(XSPF_APPLICATION).toString());

				if (xspfNamespace == XSPF_QUARKPLAYER_NAMESPACE) {
					while (!xml.atEnd() && !_stop) {
						xml.readNext();

						QString extensionElement(xml.name().toString());
						if (xml.isStartElement()) {

							if (extensionElement == XSPF_QUARKPLAYER_CUE_START_INDEX) {
								QString cueStartIndex(xml.readElementText());
								mediaInfo.setCueStartIndex(cueStartIndex);
							}

							else if (extensionElement == XSPF_QUARKPLAYER_CUE_END_INDEX) {
								QString cueEndIndex(xml.readElementText());
								mediaInfo.setCueEndIndex(cueEndIndex);
							}

							else if (extensionElement == XSPF_QUARKPLAYER_YEAR) {
								QString year(xml.readElementText());
								mediaInfo.setMetaData(MediaInfo::Year, year);
							}

							else if (extensionElement == XSPF_QUARKPLAYER_GENRE) {
								QString genre(xml.readElementText());
								mediaInfo.setMetaData(MediaInfo::Genre, genre);
							}
						}

						if (xml.isEndElement()) {
							if (extensionElement == XSPF_EXTENSION) {
								break;
							}
						}
					}
				}
			}
		}

		if (xml.isEndElement()) {
			if (element == XSPF_TRACK) {
				return;
			}
		}
	}

	if (xml.hasError()) {
		PlaylistParserWarning() << "Error:"
			<< xml.errorString()
			<< "line:" << xml.lineNumber()
			<< "column:" << xml.columnNumber();
	}
}

void XSPFParser::writeTextElement(QXmlStreamWriter & xml, const QString & qualifiedName, const QString & text) {
	if (!text.isEmpty()) {
		xml.writeTextElement(qualifiedName, text);
	}
}

void XSPFParser::writeTextElementWithNamespace(QXmlStreamWriter & xml, const QString & namespaceUri, const QString & qualifiedName, const QString & text) {
	if (!text.isEmpty()) {
		xml.writeTextElement(namespaceUri, qualifiedName, text);
	}
}

void XSPFParser::writeIntElement(QXmlStreamWriter & xml, const QString & qualifiedName, int text) {
	if (text > -1) {
		xml.writeTextElement(qualifiedName, QString::number(text));
	}
}

void XSPFParser::writeTrack(QXmlStreamWriter & xml, const MediaInfo & mediaInfo) {
	xml.writeStartElement(XSPF_TRACK);

		//Filename
		QUrl url(mediaInfo.fileName());
		writeTextElement(xml, XSPF_LOCATION, url.toEncoded());

		//Unique ID
		//FIXME not implemented yet
		//writeTextElement(xml, XSPF_IDENTIFIER, QString());

		//Artist
		writeTextElement(xml, XSPF_CREATOR, mediaInfo.metaDataValue(MediaInfo::Artist).toString());

		//Album
		writeTextElement(xml, XSPF_ALBUM, mediaInfo.metaDataValue(MediaInfo::Album).toString());

		//Track number
		writeIntElement(xml, XSPF_TRACKNUM, mediaInfo.metaDataValue(MediaInfo::TrackNumber).toInt());

		//Title
		writeTextElement(xml, XSPF_TITLE, mediaInfo.metaDataValue(MediaInfo::Title).toString());

		//Comment
		writeTextElement(xml, XSPF_ANNOTATION, mediaInfo.metaDataValue(MediaInfo::Comment).toString());

		//Length
		writeIntElement(xml, XSPF_DURATION, mediaInfo.durationMSecs());

		//Album art URL
		//FIXME not implemented yet
		//writeTextElement(xml, XSPF_IMAGE, mediaInfo.metaDataValue(MediaInfo::AlbumArt));

		//URL of the original web page
		writeTextElement(xml, XSPF_INFO, mediaInfo.metaDataValue(MediaInfo::URL).toUrl().toString());

		xml.writeStartElement(XSPF_EXTENSION);
		xml.writeAttribute(XSPF_APPLICATION, XSPF_QUARKPLAYER_NAMESPACE);
		writeTextElementWithNamespace(xml, XSPF_QUARKPLAYER_NAMESPACE, XSPF_QUARKPLAYER_CUE_START_INDEX,
			mediaInfo.cueStartIndexFormatted());
		writeTextElementWithNamespace(xml, XSPF_QUARKPLAYER_NAMESPACE, XSPF_QUARKPLAYER_CUE_END_INDEX,
			mediaInfo.cueEndIndexFormatted());
		writeTextElementWithNamespace(xml, XSPF_QUARKPLAYER_NAMESPACE, XSPF_QUARKPLAYER_YEAR,
			mediaInfo.metaDataValue(MediaInfo::Year).toDate().toString("yyyy"));
		writeTextElementWithNamespace(xml, XSPF_QUARKPLAYER_NAMESPACE, XSPF_QUARKPLAYER_GENRE,
			mediaInfo.metaDataValue(MediaInfo::Genre).toString());
		xml.writeEndElement();	//extension

	xml.writeEndElement();	//track
}

bool XSPFParser::load(const QString & location) {
	QIODevice * device = Util::openLocationReadMode(location);
	if (!device) {
		return false;
	}

	_stop = false;

	QList<MediaInfo> files;

	MediaInfo mediaInfo;
	QString base;

	QXmlStreamReader xml(device);
	while (!xml.atEnd() && !_stop) {
		xml.readNext();

		switch (xml.tokenType()) {

		case QXmlStreamReader::StartElement: {
			QString element(xml.name().toString());

			if (element == XSPF_PLAYLIST) {
				QXmlStreamAttributes attributes = xml.attributes();

				//Base
				if (attributes.hasAttribute(XSPF_BASE)) {
					base = attributes.value(XSPF_BASE).toString();
				}
			}

			else if (element == XSPF_TRACK) {
				readTrack(xml, mediaInfo, base);

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

			//Otherwise won't read the track end element
			break;
		}

		default:
			//Do nothing
			break;

		}
	}

	if (xml.hasError()) {
		PlaylistParserWarning() << "Error:" << xml.errorString()
			<< "line:" << xml.lineNumber()
			<< "column:" << xml.columnNumber();
	}

	device->close();
	delete device;

	if (!files.isEmpty()) {
		//Emits the signal for the remaining files found (< FILES_FOUND_LIMIT)
		emit filesFound(files);
	}

	return true;
}

bool XSPFParser::save(const QString & location, const QList<MediaInfo> & files) {
	QIODevice * device = Util::openLocationWriteMode(location);
	if (!device) {
		return false;
	}

	_stop = false;

	QXmlStreamWriter xml(device);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();

	xml.writeStartElement(XSPF_PLAYLIST);
	xml.writeAttribute("version", "1");
	xml.writeDefaultNamespace("http://xspf.org/ns/0/");
	xml.writeNamespace(XSPF_QUARKPLAYER_NAMESPACE, "qp");

		writeTextElement(xml, XSPF_TITLE, QFileInfo(location).completeBaseName());
		writeTextElement(xml, XSPF_CREATOR, QCoreApplication::applicationName());
		writeTextElement(xml, XSPF_INFO, "http://quarkplayer.googlecode.com");
		writeTextElement(xml, XSPF_DATE, QDateTime::currentDateTime().toString(Qt::ISODate));

		xml.writeStartElement(XSPF_TRACKLIST);

			foreach (MediaInfo mediaInfo, files) {
				if (_stop) {
					break;
				}
				writeTrack(xml, mediaInfo);
			}

		xml.writeEndElement();	//trackList

	xml.writeEndElement();	//playlist

	device->close();
	delete device;

	return true;
}
