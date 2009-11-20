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

#ifndef OPENSUBTITLESPARSER_H
#define OPENSUBTITLESPARSER_H

#include <QtCore/QList>
#include <QtCore/QString>

class QByteArray;

/**
 * Parses the web service/XML from the website http://www.opensubtitles.org
 *
 * http://www.opensubtitles.org gives us a subtitle file given a movie filename.
 *
 * Source code adapted from SMPlayer, original author: Ricardo Villalba
 *
 * @author Ricardo Villalba
 * @author Tanguy Krotoff
 */
class OpenSubtitlesParser {
public:

	/**
	 * Represents a subtitle has read from OpenSubtitles.org XML.
	 */
	struct Subtitle {
		QString movie;
		QString releaseName;
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
	 * Parses the XML given by OpenSubtitles.org web service.
	 *
	 * @param xml XML
	 * @return the list of subtitles
	 */
	static QList<Subtitle> parseXml(const QByteArray & xml);

	/**
	 * Computes a hash from a movie file name that identifies uniquely the movie.
	 *
	 * This is specific to OpenSubtitles.org
	 *
	 * Patch by Kamil Dziobek
	 * Copyright (C) 2008  Kamil Dziobek <turbos11@gmail.com>
	 * License: BSD or GPL or public domain
	 *
	 * @see http://trac.opensubtitles.org/projects/opensubtitles/wiki/HashSourceCodes
	 */
	static QString calculateHash(const QString & fileName);

private:

	OpenSubtitlesParser();

	~OpenSubtitlesParser();
};

#endif	//OPENSUBTITLESPARSER_H
