/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef CUEPARSER_H
#define CUEPARSER_H

#include "IPlaylistParserImpl.h"

#include <QtCore/QString>

/**
 * Parses a CUE sheet.
 *
 * @see http://en.wikipedia.org/wiki/Cue_sheet_(computing)
 * @see http://digitalx.org/cuesheetsyntax.php
 * @author Tanguy Krotoff
 */
class CUEParser : public IPlaylistParserImpl {
public:

	CUEParser(QObject * parent);

	~CUEParser();

	QStringList fileExtensions() const;

	bool load(const QString & location);

	bool save(const QString & location, const QList<MediaInfo> & files);

	void stop();

private:

	volatile bool _stop;
};

#endif	//CUEPARSER_H
