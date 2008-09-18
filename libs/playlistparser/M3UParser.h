/*
 * QuarkPlayer, a Phonon media player
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

#ifndef M3UPARSER_H
#define M3UPARSER_H

#include "PlaylistParser.h"

#include <QtCore/QString>

/**
 * Parses a m3u/m3u8 playlist.
 *
 * @author Tanguy Krotoff
 */
class M3UParser : public IPlaylistParser {
public:

	M3UParser(const QString & filename, QObject * parent);

	~M3UParser();

	QStringList fileExtensions() const;

	void load();

	void save(const QStringList & files);

private:

	static QString changeSlashes(const QString & filename);

	bool isUtf8() const;

	QString _filename;
};

#endif	//M3UPARSER_H
