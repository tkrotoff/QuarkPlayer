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

#include "XSPFParser.h"

#include "Util.h"

#include <mediainfofetcher/MediaInfo.h>
#include <tkutil/TkFile.h>

#include <QtXml/QXmlStreamReader>
#include <QtXml/QXmlStreamWriter>

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtCore/QTime>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>

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

//Specific to foobar2000 XSPF pluin
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

XSPFParser::XSPFParser(const QString & filename, QObject * parent)
	: IPlaylistParser(filename, parent) {

	_filename = filename;
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

void XSPFParser::readTrack(QXmlStreamReader & xml, MediaInfo & mediaInfo) const {
	while (!xml.atEnd() && !_stop) {
		xml.readNext();

		QString element(xml.name().toString());

		if (xml.isStartElement()) {

			//Filename
			if (element == XSPF_LOCATION) {
				QUrl url = QUrl::fromEncoded(xml.readElementText().toUtf8());
				QString location(url.toString());
				bool isUrl = MediaInfo::isUrl(location);
				mediaInfo.setUrl(isUrl);
				if (isUrl) {
					mediaInfo.setFileName(location);
				} else {
					QString path(QFileInfo(_filename).path());
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
				mediaInfo.insertMetadata(MediaInfo::Artist, creator);
			}

			//Album
			else if (element == XSPF_ALBUM) {
				QString album(xml.readElementText());
				mediaInfo.insertMetadata(MediaInfo::Album, album);
			}

			//Track number
			else if (element == XSPF_TRACKNUM) {
				QString trackNum(xml.readElementText());
				mediaInfo.insertMetadata(MediaInfo::TrackNumber, trackNum);
			}

			//Title
			else if (element == XSPF_TITLE) {
				QString title(xml.readElementText());
				mediaInfo.insertMetadata(MediaInfo::Title, title);
			}

			//Comment
			else if (element == XSPF_ANNOTATION) {
				QString annotation(xml.readElementText());
				if (mediaInfo.metadataValue(MediaInfo::Title).isEmpty()) {
					//Some people didn't understand how XSPF works
					//and confused annotation with title
					mediaInfo.insertMetadata(MediaInfo::Title, annotation);
				}
				mediaInfo.insertMetadata(MediaInfo::Comment, annotation);
			}

			//Length
			else if (element == XSPF_DURATION) {
				int duration = xml.readElementText().toInt();
				//XSPF gives us the duration in milliseconds
				//Let's convert it to seconds
				mediaInfo.setLength(duration / 1000);
			}

			//Album art URL
			else if (element == XSPF_IMAGE) {
				QString image(xml.readElementText());
				//FIXME not implemented yet
				//mediaInfo.insertMetadata(MediaInfo::AlbumArt, image);
			}

			//URL of the original web page
			else if (element == XSPF_INFO) {
				QString info(xml.readElementText());
				mediaInfo.insertMetadata(MediaInfo::URL, info);
			}

			//Meta
			else if (element == XSPF_META) {

				//These tags are specific to foobar2000 XSPF plugin

				QXmlStreamAttributes attributes = xml.attributes();

				//Date
				if (attributes.hasAttribute(XSPF_FOOBAR2000_DATE)) {
					QString date(attributes.value(XSPF_FOOBAR2000_DATE).toString());
					mediaInfo.insertMetadata(MediaInfo::Year, date);
				}

				//Genre
				else if (attributes.hasAttribute(XSPF_FOOBAR2000_GENRE)) {
					QString genre(attributes.value(XSPF_FOOBAR2000_GENRE).toString());
					mediaInfo.insertMetadata(MediaInfo::Genre, genre);
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
								mediaInfo.insertMetadata(MediaInfo::Year, year);
							}

							else if (extensionElement == XSPF_QUARKPLAYER_GENRE) {
								QString genre(xml.readElementText());
								mediaInfo.insertMetadata(MediaInfo::Genre, genre);
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
		writeTextElement(xml, XSPF_CREATOR, mediaInfo.metadataValue(MediaInfo::Artist));

		//Album
		writeTextElement(xml, XSPF_ALBUM, mediaInfo.metadataValue(MediaInfo::Album));

		//Track number
		writeTextElement(xml, XSPF_TRACKNUM, mediaInfo.metadataValue(MediaInfo::TrackNumber));

		//Title
		writeTextElement(xml, XSPF_TITLE, mediaInfo.metadataValue(MediaInfo::Title));

		//Comment
		writeTextElement(xml, XSPF_ANNOTATION, mediaInfo.metadataValue(MediaInfo::Comment));

		//Length
		writeIntElement(xml, XSPF_DURATION, mediaInfo.lengthMilliseconds());

		//Album art URL
		//FIXME not implemented yet
		//writeTextElement(xml, XSPF_IMAGE, mediaInfo.metadataValue(MediaInfo::AlbumArt));

		//URL of the original web page
		writeTextElement(xml, XSPF_INFO, mediaInfo.metadataValue(MediaInfo::URL));

		xml.writeStartElement(XSPF_EXTENSION);
		xml.writeAttribute(XSPF_APPLICATION, XSPF_QUARKPLAYER_NAMESPACE);
		writeTextElementWithNamespace(xml, XSPF_QUARKPLAYER_NAMESPACE, XSPF_QUARKPLAYER_CUE_START_INDEX, mediaInfo.cueStartIndexFormatted());
		writeTextElementWithNamespace(xml, XSPF_QUARKPLAYER_NAMESPACE, XSPF_QUARKPLAYER_CUE_END_INDEX, mediaInfo.cueEndIndexFormatted());
		writeTextElementWithNamespace(xml, XSPF_QUARKPLAYER_NAMESPACE, XSPF_QUARKPLAYER_YEAR, mediaInfo.metadataValue(MediaInfo::Year));
		writeTextElementWithNamespace(xml, XSPF_QUARKPLAYER_NAMESPACE, XSPF_QUARKPLAYER_GENRE, mediaInfo.metadataValue(MediaInfo::Genre));
		xml.writeEndElement();	//extension

	xml.writeEndElement();	//track
}

void XSPFParser::load() {
	QTime timeElapsed;
	timeElapsed.start();

	_stop = false;

	QList<MediaInfo> files;

	qDebug() << __FUNCTION__ << "Playlist:" << _filename;

	QFile file(_filename);
	if (file.open(QIODevice::ReadOnly)) {
		MediaInfo mediaInfo;

		QXmlStreamReader xml(&file);
		while (!xml.atEnd() && !_stop) {
			xml.readNext();

			switch (xml.tokenType()) {

			case QXmlStreamReader::StartElement: {
				QString element(xml.name().toString());

				if (element == XSPF_TRACK) {
					readTrack(xml, mediaInfo);

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

			}
		}

		if (xml.hasError()) {
			qCritical() << __FUNCTION__ << "Error:" << xml.errorString()
				<< "line:" << xml.lineNumber()
				<< "column:" << xml.columnNumber();
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

void XSPFParser::save(const QList<MediaInfo> & files) {
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

		xml.writeStartElement(XSPF_PLAYLIST);
		xml.writeAttribute("version", "1");
		xml.writeDefaultNamespace("http://xspf.org/ns/0/");
		xml.writeNamespace(XSPF_QUARKPLAYER_NAMESPACE, "qp");

			writeTextElement(xml, XSPF_TITLE, QFileInfo(_filename).baseName());
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
	}

	file.close();

	//Emits the last signal
	emit finished(timeElapsed.elapsed());
}
