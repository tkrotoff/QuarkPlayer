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

#ifndef ASXPARSER_H
#define ASXPARSER_H

#include "PlaylistParser.h"

#include <QtCore/QString>

/**
 * Parses a ASX playlist (Advanced Stream Redirector).
 *
 * ASX is UTF-8 encoded?
 *
 * @see http://en.wikipedia.org/wiki/Advanced_Stream_Redirector
 * @author Tanguy Krotoff
 */
class ASXParser : public IPlaylistParser {
public:

	ASXParser(const QString & filename, QObject * parent);

	~ASXParser();

	QStringList fileExtensions() const;

	void load();

	void save(const QList<MediaInfo> & files);

	void stop();

private:

	QString _filename;

	volatile bool _stop;
};

#endif	//ASXPARSER_H
