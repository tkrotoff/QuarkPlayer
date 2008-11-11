/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2008  Kamil Dziobek <turbos11@gmail.com>
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

#ifndef OPENSUBTITLESPARSER_H
#define OPENSUBTITLESPARSER_H

#include <QtXml/QDomDocument>

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>

struct OpenSubtitlesSubtitle {
	QString movie;
	QString releasename;
	QString link;
	QString iso639;
	QString language;
	QString date;
	QString format;
	QString comments;
	QString detail;
	QString rating;
	QString files;
	QString user;
};

/**
 * Parses the web service from the website http://www.opensubtitles.org
 *
 * http://www.opensubtitles.org gives us a subtitle file given a movie filename.
 *
 * Source code taken from SMPlayer, original author: Ricardo Villalba
 *
 * @author Ricardo Villalba
 * @author Tanguy Krotoff
 */
class OpenSubtitlesParser {
public:

	OpenSubtitlesParser();

	~OpenSubtitlesParser();

	bool parseXml(const QByteArray & xml);

	QList<OpenSubtitlesSubtitle> subtitleList() const;

	/**
	 * Patch by Kamil Dziobek
	 * Copyright (C) 2008  Kamil Dziobek <turbos11@gmail.com>
	 * License: BSD or GPL or public domain
	 */
	static QString calculateHash(const QString & fileName);

private:

	QDomDocument _doc;

	QList<OpenSubtitlesSubtitle> _subtitles;
};

#endif	//OPENSUBTITLESPARSER_H
