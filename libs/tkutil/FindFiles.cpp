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

#include "FindFiles.h"

#include "TkFile.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

#include <dirent.h>
#ifdef Q_OS_WIN
	#include <direct.h>
#endif	//Q_OS_WIN

static const int FILES_FOUND_LIMIT = 500;

FindFiles::FindFiles(QObject * parent)
	: QObject(parent) {
}

FindFiles::~FindFiles() {
}

void FindFiles::setSearchPath(const QString & path) {
	_path = path;
}

void FindFiles::findAllFiles() {
	if (_path.isEmpty()) {
		qCritical() << __FUNCTION__ << "Error: empty path";
		return;
	}

	_currentPath.clear();
	findAllFiles(_path);

	if (!_files.isEmpty()) {
		emit filesFound(_files);
		_files.clear();
	}

	//Emits the last signal
	emit finished();
}

void FindFiles::findAllFiles(const QString & path) {
	DIR * dir = opendir(path.toUtf8().constData());
	if (dir != NULL) {

		if (_currentPath.isEmpty()) {
			_currentPath = path;
		} else {
			_currentPath += '/' + path;
		}

		chdir(path.toUtf8().constData());

		struct dirent * entry = NULL;
		while (entry = readdir(dir)) {
			QString name(entry->d_name);

			//Avoid '.', '..' and other hidden files
			if (!name.startsWith('.')) {

				if (TkFile::isDir(name)) {
					findAllFiles(name);
				}

				else {
					if (_currentPath.contains("C:ocuments")) {
						qDebug() << _currentPath + '/' + name;
					}

					//qDebug() << _currentPath + '/' + name;
					_files << _currentPath + '/' + name;

					if (_files.size() > FILES_FOUND_LIMIT) {
						//Emits the signal every FILES_FOUND_LIMIT files found
						emit filesFound(_files);
						_files.clear();
					}
				}
			}
		}

		QString tmp('/' + path);
		int pos = _currentPath.lastIndexOf(tmp);
		_currentPath.remove(pos, tmp.length());

		chdir("..");

		closedir(dir);
	}
}
