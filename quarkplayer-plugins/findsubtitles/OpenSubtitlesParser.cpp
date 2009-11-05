/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2008  Kamil Dziobek <turbos11@gmail.com>
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

#include "OpenSubtitlesParser.h"

#include <QtCore/QFile>
#include <QtCore/QDataStream>
#include <QtCore/QDebug>

OpenSubtitlesParser::OpenSubtitlesParser() {
}

OpenSubtitlesParser::~OpenSubtitlesParser() {
}

bool OpenSubtitlesParser::parseXml(const QByteArray & xml) {
	//qDebug() << __FUNCTION__ << "OpenSubtitles XML:" << xml.constData();

	_subtitles.clear();

	bool ok = _doc.setContent(xml);
	if (!ok) {
		qCritical() << __FUNCTION__ << "Error: couldn't parse the given XML";
		return false;
	}

	QDomNode root = _doc.documentElement();

	QString baseUrl = root.firstChildElement("base").text();

	QDomNode child = root.firstChildElement("results");
	if (!child.isNull()) {
		QDomNode subtitle = child.firstChildElement("subtitle");
		while (!subtitle.isNull()) {
			OpenSubtitlesSubtitle sub;

			sub.releasename = subtitle.firstChildElement("releasename").text();
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

			_subtitles.append(sub);

			subtitle = subtitle.nextSiblingElement("subtitle");
		}
	}

	return true;
}

QList<OpenSubtitlesSubtitle> OpenSubtitlesParser::subtitleList() const {
	return _subtitles;
}

QString OpenSubtitlesParser::calculateHash(const QString & fileName) {
	QFile file(fileName);
	QString hexHash;

	if (!file.exists()) {
		qCritical() << __FUNCTION__ << "Error: file doesn't exist: " << fileName;
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
