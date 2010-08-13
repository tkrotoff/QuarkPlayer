/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2008  Kamil Dziobek <turbos11@gmail.com>
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "OpenSubtitlesParser.h"

#include "FindSubtitlesLogger.h"

#include <QtXml/QDomDocument>

#include <QtCore/QFile>
#include <QtCore/QDataStream>

QList<OpenSubtitlesParser::Subtitle> OpenSubtitlesParser::parseXml(const QByteArray & xml) {
	//FindSubtitlesDebug() << "OpenSubtitles.org XML:" << xml.constData();

	QList<Subtitle> subtitles;

	QDomDocument doc;
	bool ok = doc.setContent(xml);
	if (!ok) {
		FindSubtitlesCritical() << "Couldn't parse the given XML";
		return subtitles;
	}

	QDomNode root = doc.documentElement();

	QString baseUrl = root.firstChildElement("base").text();

	QDomNode child = root.firstChildElement("results");
	if (!child.isNull()) {
		QDomNode subtitle = child.firstChildElement("subtitle");
		while (!subtitle.isNull()) {
			Subtitle sub;

			sub.releaseName = subtitle.firstChildElement("releasename").text();
			sub.link = baseUrl + subtitle.firstChildElement("download").text();
			sub.detail = subtitle.firstChildElement("detail").text();
			sub.date = subtitle.firstChildElement("subadddate").text();
			sub.rating = subtitle.firstChildElement("subrating").text();
			sub.comments = subtitle.firstChildElement("subcomments").text();
			sub.movie = subtitle.firstChildElement("movie").text();
			sub.files = subtitle.firstChildElement("files").text();
			sub.format = subtitle.firstChildElement("format").text();
			sub.language = subtitle.firstChildElement("language").text();
			sub.iso639 = subtitle.firstChildElement("iso639").text();
			sub.user = subtitle.firstChildElement("user").text();

			subtitles.append(sub);

			subtitle = subtitle.nextSiblingElement("subtitle");
		}
	}

	return subtitles;
}

QString OpenSubtitlesParser::calculateHash(const QString & fileName) {
	QFile file(fileName);
	QString hexHash;

	if (!file.exists()) {
		FindSubtitlesCritical() << "File doesn't exist: " << fileName;
		return hexHash;
	}

	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);
	in.setByteOrder(QDataStream::LittleEndian);
	quint64 size = file.size();
	quint64 hash = size;
	quint64 a;
	for (int i = 0; i < 8192; i++) {
		in >> a;
		hash += a;
	};
	file.seek(size - 65536);
	for (int i = 0; i < 8192; i++) {
		in >> a;
		hash += a;
	};

	hexHash.setNum(hash, 16);

	return hexHash;
}
