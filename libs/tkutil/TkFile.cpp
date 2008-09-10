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

#include "TkFile.h"

#include <QtCore/QDebug>

#include <sys/types.h>
#include <sys/stat.h>

#ifndef S_ISDIR
	#ifdef S_IFDIR
		#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
	#else
		#define S_ISDIR(mode) 0
	#endif
#endif	//!S_ISDIR

QString TkFile::fileName(const QString & path) {
	return path.right(path.length() - path.lastIndexOf('/') - 1);
}

QString TkFile::dir(const QString & filename) {
	QString left(filename.left(filename.lastIndexOf('/')));
	return left.right(left.length() - left.lastIndexOf('/') - 1);
}

QString TkFile::removeFileExtension(const QString & path) {
	int dotLastIndex = path.lastIndexOf('.');
	return path.left(dotLastIndex);
}

QString TkFile::fileExtension(const QString & path) {
	int dotLastIndex = path.lastIndexOf('.');
	if (dotLastIndex == -1) {
		return QString();
	} else {
		return path.right(path.length() - dotLastIndex - 1);
	}
}

QString TkFile::path(const QString & filename) {
	return filename.left(filename.lastIndexOf('/'));
}

bool TkFile::isDir(const QString & path) {
	struct stat statbuf;

	stat(path.toUtf8().constData(), &statbuf);
	return S_ISDIR(statbuf.st_mode);
}
