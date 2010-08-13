/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "Util.h"

#include "PlaylistParserLogger.h"

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

QString Util::pathToNativeSeparators(const QString & path) {
	QString tmp(path);

	//Only change if the file or path exists (if it's a local file)
	if (QFileInfo(tmp).exists()) {
		tmp = QDir::toNativeSeparators(tmp);
	}

	return tmp;
}

QString Util::canonicalFilePath(const QString & path, const QString & fileName) {
	QString tmp(fileName);

	QFileInfo fileInfo(tmp);
	if (fileInfo.exists()) {
		tmp = fileInfo.absoluteFilePath();
	} else {
		QString tmp2(tmp);

		//This does not work with Qt 4.6.2 under Ubuntu 9.10
		//Let's do it manually
		//tmp2 = QDir::toNativeSeparators(tmp2);
		tmp2.replace("\\", "/");

		fileInfo = QFileInfo(path + QDir::separator() + tmp2);
		if (fileInfo.exists()) {
			tmp = fileInfo.canonicalFilePath();
		}
	}

	return tmp;
}
