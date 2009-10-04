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

#ifndef IPLAYLISTPARSERIMPL_H
#define IPLAYLISTPARSERIMPL_H

#include "IPlaylistParser.h"

class MediaInfo;

class QIODevice;
class QString;
class QStringList;

/**
 * Interface for saving a playlist file.
 *
 * @author Tanguy Krotoff
 */
class IPlaylistParserImpl : public IPlaylistParser {
	Q_OBJECT
public:

	/** Files found limit before to send the signal filesFound(). */
	static const int FILES_FOUND_LIMIT = 1000;

	IPlaylistParserImpl(QObject * parent);

	virtual ~IPlaylistParserImpl();

	/**
	 * Returns the list of file extensions supported by this parser.
	 *
	 * @return list of file extensions (i.e m3u, pls...)
	 */
	virtual QStringList fileExtensions() const = 0;

	virtual void load(QIODevice * device, const QString & location) = 0;

	virtual void save(QIODevice * device, const QString & location, const QList<MediaInfo> & files) = 0;

signals:

	/**
	 * @see PlaylistReader::filesFound()
	 */
	void filesFound(const QList<MediaInfo> & files);

};

#endif	//IPLAYLISTPARSERIMPL_H
