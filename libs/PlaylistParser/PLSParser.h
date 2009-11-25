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

#ifndef PLSPARSER_H
#define PLSPARSER_H

#include "IPlaylistParserImpl.h"

#include <QtCore/QString>

/**
 * Parses a PLS playlist.
 *
 * @see http://en.wikipedia.org/wiki/PLS_(file_format)
 * @author Tanguy Krotoff
 */
class PLSParser : public IPlaylistParserImpl {
public:

	PLSParser(QObject * parent);

	~PLSParser();

	QStringList fileExtensions() const;

	void load(QIODevice * device, const QString & location);

	void save(QIODevice * device, const QString & location, const QList<MediaInfo> & files);

	void stop();

private:

	volatile bool _stop;
};

#endif	//PLSPARSER_H
