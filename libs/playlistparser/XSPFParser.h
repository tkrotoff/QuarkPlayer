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

#ifndef XSPFPARSER_H
#define XSPFPARSER_H

#include "PlaylistParser.h"

#include <QtCore/QString>

class QXmlStreamReader;
class QXmlStreamWriter;

/**
 * Parses a XML Shareable Playlist Format (XSPF) playlist.
 *
 * XSPF, pronounced spiff, is an XML-based playlist format for digital media, sponsored by the Xiph.Org Foundation.
 * Format created in 2004.
 *
 * Default QuarkPlayer playlist format.
 *
 * Future features:
 * Add QuarkPlayer extension in order to save (and import) the SQL database inside a XSPF file.
 * Amarok use a XML file format to load/save its SQL database,
 * see http://websvn.kde.org/trunk/extragear/multimedia/amarok/utilities/collectionscanner/
 *
 * @see http://en.wikipedia.org/wiki/XML_Shareable_Playlist_Format
 * @see http://xspf.org/
 * @see http://xspf.org/xspf-v1.html
 * @author Tanguy Krotoff
 */
class XSPFParser : public IPlaylistParser {
public:

	XSPFParser(const QString & filename, QObject * parent);

	~XSPFParser();

	QStringList fileExtensions() const;

	void load();

	void save(const QList<MediaInfo> & files);

	void stop();

private:

	void readTrack(QXmlStreamReader & xml, MediaInfo & mediaInfo) const;

	static void writeTextElement(QXmlStreamWriter & xml, const QString & qualifiedName, const QString & text);

	static void writeTextElementWithNamespace(QXmlStreamWriter & xml, const QString & namespaceUri, const QString & qualifiedName, const QString & text);

	static void writeIntElement(QXmlStreamWriter & xml, const QString & qualifiedName, int text);

	static void writeTrack(QXmlStreamWriter & xml, const MediaInfo & mediaInfo);

	QString _filename;

	volatile bool _stop;
};

#endif	//XSPFPARSER_H
