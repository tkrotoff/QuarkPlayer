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

#ifndef OPENSUBTITLESPARSER_H
#define OPENSUBTITLESPARSER_H

#include <QtCore/QList>
#include <QtCore/QString>

class QByteArray;

/**
 * Parses the web service/XML from OpenSubtitles.org
 *
 * http://www.opensubtitles.org gives us a list of subtitles given a movie filename.
 *
 * Source code adapted from SMPlayer, original author: Ricardo Villalba
 *
 * @author Ricardo Villalba
 * @author Tanguy Krotoff
 */
class OpenSubtitlesParser {
public:

	/**
	 * Represents a subtitle inside OpenSubtitles.org XML format.
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
	 * Parses the XML provided by OpenSubtitles.org web service.
	 *
	 * Example of (truncated) OpenSubtitles.org XML format
	 * Request URL is: http://www.opensubtitles.org/en/search/sublanguageid-all/moviehash-55fa82a34fd38763/simplexml
	 * With movie being: Elephants Dream, elephantsdream-1920-hd-mpeg4-su-ac3.avi
	 * @code
	 * <?xml version="1.0" encoding="utf-8"?>
	 * <search>
	 * 	<base>http://www.opensubtitles.org/en</base>
	 * 	<results items='10' itemsfound='10' searchtime='0.196'>
	 * 		<subtitle>
	 * 			<download>/download/sub/3303153</download>
	 * 			<detail>/subtitles/3303153/elephants-dream-en</detail>
	 * 			<iso639>en</iso639>
	 * 			<user>one4all</user>
	 * 			<releasename><![CDATA[Elephants Dream [2006]]]></releasename>
	 * 			<idsubtitle>3303153</idsubtitle>
	 * 			<subadddate>2008-07-10 20:08:05</subadddate>
	 * 			<subrating>0.0</subrating>
	 * 			<subcomments><![CDATA[0]]></subcomments>
	 * 			<movie><![CDATA[Elephants Dream (2006)]]></movie>
	 * 			<files>1</files>
	 * 			<format>srt</format>
	 * 			<language>English</language>
	 * 		</subtitle>
	 * 		<subtitle>
	 * 			<download>/download/sub/3180984</download>
	 * 			<detail>/subtitles/3180984/elephants-dream-fr</detail>
	 * 			<iso639>fr</iso639>
	 * 			<user>jezusazerty</user>
	 * 			<releasename><![CDATA[Helephants Dream]]></releasename>
	 * 			<idsubtitle>3180984</idsubtitle>
	 * 			<subadddate>2008-01-27 22:55:30</subadddate>
	 * 			<subrating>0.0</subrating>
	 * 			<subcomments><![CDATA[0]]></subcomments>
	 * 			<movie><![CDATA[Elephants Dream (2006)]]></movie>
	 * 			<files>1</files>
	 * 			<format>srt</format>
	 * 			<language>French</language>
	 * 		</subtitle>
	 * 		<subtitle>
	 * 			<download>/download/sub/3093137</download>
	 * 			<detail>/subtitles/3093137/elephants-dream-es</detail>
	 * 			<iso639>es</iso639>
	 * 			<user>tuxsoul</user>
	 * 			<releasename><![CDATA[Elephants Dream]]></releasename>
	 * 			<idsubtitle>3093137</idsubtitle>
	 * 			<subadddate>2007-01-01 10:15:30</subadddate>
	 * 			<subrating>0.0</subrating>
	 * 			<subcomments><![CDATA[0]]></subcomments>
	 * 			<movie><![CDATA[Elephants Dream (2006)]]></movie>
	 * 			<files>1</files>
	 * 			<format>srt</format>
	 * 			<language>Spanish</language>
	 * 		</subtitle>
	 * 	</results>
	 * </search>
	 * @endcode
	 *
	 * @param xml OpenSubtitles.org XML format to parse
	 * @return the list of subtitles
	 */
	static QList<Subtitle> parseXml(const QByteArray & xml);

	/**
	 * Computes a hash given a movie file name that identifies uniquely the movie.
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
