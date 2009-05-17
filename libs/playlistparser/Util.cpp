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

#include "Util.h"

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDebug>

QString Util::pathToNativeSeparators(const QString & path) {
	QString tmp(path);

	//Only change if the file or path exists (if it's a local file)
	if (QFileInfo(tmp).exists()) {
		tmp = QDir::toNativeSeparators(tmp);
	}

	return tmp;
}

QString Util::canonicalFilePath(const QString & path, const QString & filename) {
	QString tmp(filename);

	QFileInfo fileInfo(tmp);
	if (fileInfo.exists()) {
		tmp = fileInfo.absoluteFilePath();
	} else {
		fileInfo = QFileInfo(path + QDir::separator() + tmp);
		if (fileInfo.exists()) {
			tmp = fileInfo.canonicalFilePath();
		}
	}

	return tmp;
}

QString Util::relativeFilePath(const QString & path, const QString & filename) {
	//FIXME Yes this can be improved
	//Does not handle this case for example:
	//path = "C:\Documents and Settings\tanguy_k\My Documents\My Music\My Playlists\"
	//filename = "C:/Documents and Settings/tanguy_k/My Documents/Downloads/Cindy Sander - Papillon de lumiere 3'37.mp3"
	//result = "..\..\Downloads\Cindy Sander - Papillon de lumiere 3'37.mp3"

	QString tmp(filename);

	QString pathTmp(path + '/');
	if (tmp.startsWith(pathTmp)) {
		tmp = tmp.mid(pathTmp.length());
	}

	return tmp;
}
