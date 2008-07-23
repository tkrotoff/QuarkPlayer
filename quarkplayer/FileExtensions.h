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

#ifndef FILEEXTENSIONS_H
#define FILEEXTENSIONS_H

#include <quarkplayer/quarkplayer_export.h>

#include <QtCore/QStringList>

/**
 * File extensions supported by QuarkPlayer.
 *
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API FileExtensions {
public:

	static QStringList video();

	static QStringList audio();

	static QStringList multimedia();

	static QStringList subtitle();

	static QStringList playlist();

	/**
	 * When opening a file, we need a file format filter.
	 *
	 * @see QFileDialog
	 */
	static QString toFilterFormat(const QStringList & extensions);

private:

	FileExtensions();

	static QStringList _videoExtensions;

	static QStringList _audioExtensions;

	static QStringList _multimediaExtensions;

	static QStringList _subtitleExtensions;

	static QStringList _playlistExtensions;
};

#endif	//FILEEXTENSIONS_H
