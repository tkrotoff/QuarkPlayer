/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <QtCore/QDir>
#include <QtCore/QDebug>

#include <sys/types.h>
#include <sys/stat.h>

#include <cerrno>

bool TkFile::isDir(const QString & path) {
	bool dir = false;

#ifdef Q_CC_MSVC
	//See http://msdn.microsoft.com/en-us/library/14h5k7ff.aspx
	//_wstat does not work with Windows Vista symbolic links.
	//In these cases, _wstat will always report a file size of 0.
	//_stat does work correctly with symbolic links.

	//_wstati64 is used by Qt 3.3.x (qfileinfo_win.cpp) so let's use the same wstat() function
	//See http://www.google.com/codesearch/

	const wchar_t * encodedName = reinterpret_cast<const wchar_t *>(path.utf16());
	struct _stati64 statbuf;
	int error = _wstati64(encodedName, &statbuf);
	if (error != 0) {
		qCritical() << Q_FUNC_INFO << "_wstati64() failed, path:" << path << "errno:" << strerror(errno);
	} else {
		dir = (statbuf.st_mode & S_IFMT) == S_IFDIR;
	}
#else
	struct stat statbuf;
	int error = stat(QFile::encodeName(path), &statbuf);
	if (error != 0) {
		qCritical() << Q_FUNC_INFO << "stat() failed, path:" << path << "errno:" << strerror(errno);
	} else {
		dir = S_ISDIR(statbuf.st_mode);
	}
#endif	//Q_CC_MSVC

	return dir;
}

QString TkFile::relativeFilePath(const QString & path, const QString & fileName) {
	//FIXME Yes this can be improved
	//Does not handle this case for example:
	//path = "C:\Documents and Settings\tanguy_k\My Documents\My Music\My Playlists\"
	//filename = "C:/Documents and Settings/tanguy_k/My Documents/Downloads/Cindy Sander - Papillon de lumiere 3'37.mp3"
	//result = "..\..\Downloads\Cindy Sander - Papillon de lumiere 3'37.mp3"

	QString tmp(fileName);
	tmp = QDir::toNativeSeparators(tmp);

	QString pathTmp(path + '/');
	pathTmp = QDir::toNativeSeparators(pathTmp);
	if (tmp.startsWith(pathTmp)) {
		tmp = tmp.mid(pathTmp.length());
	}

	return tmp;
}
