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

#include "TkFile.h"

#include <QtCore/QDebug>

#include <sys/types.h>
#include <sys/stat.h>

bool TkFile::isDir(const QString & path) {
	const wchar_t * encodedName = reinterpret_cast<const wchar_t *>(path.utf16());

#ifdef Q_CC_MSVC
	struct _stat statbuf;
	_wstat(encodedName, &statbuf);
	return ((statbuf.st_mode & S_IFMT) == S_IFDIR);
#else
	struct stat statbuf;
	lstat(encodedName, &statbuf);
	return S_ISDIR(statbuf.st_mode);
#endif	//Q_CC_MSVC
}
