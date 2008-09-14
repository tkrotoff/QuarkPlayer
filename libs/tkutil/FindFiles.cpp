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

static const int DEFAULT_FILES_FOUND_LIMIT = 500;

FindFiles::FindFiles(QObject * parent)
	: QObject(parent) {

	_findDirs = false;
	_filesFoundLimit = DEFAULT_FILES_FOUND_LIMIT;
}

FindFiles::~FindFiles() {
}

void FindFiles::setSearchPath(const QString & path) {
	_path = path;
}

void FindFiles::setFilesFoundLimit(int filesFoundLimit) {
	_filesFoundLimit = filesFoundLimit;
}

void FindFiles::findAllFiles(const QRegExp & pattern, const QStringList & extensions) {
	_pattern = pattern;
	_extensions = extensions;

	if (_path.isEmpty()) {
		qCritical() << __FUNCTION__ << "Error: empty path";
		return;
	}

	_currentPath.clear();
	findAllFilesInternal(_path);

	if (!_files.isEmpty()) {
		emit filesFound(_files);
		_files.clear();
	}

	//Emits the last signal
	emit finished();
}

void FindFiles::findAllFilesAndDirs(const QRegExp & pattern, const QStringList & extensions) {
	_findDirs = true;
	findAllFiles(pattern, extensions);
}

void FindFiles::findAllFilesInternal(const QString & path) {

	//Warning: opendir() is limited to MAX_PATH i.e 255 characters
	//This is a pretty stupid limitation
	//chdir() helps to overcome this limitation
	DIR * dir = opendir(path.toUtf8().constData());

	if (!dir) {
		qCritical() << __FUNCTION__ << "Error: opendir() failed";
		perror(path.toUtf8().constData());
	} else {
		if (_currentPath.isEmpty()) {
			_currentPath = path;
		} else {
			_currentPath += '/' + path;
		}
		_currentPath.replace("//", "/");

		//Found a crazy bug!
		//Under Windows it seems that doing chdir("Documents and Settings")
		//you go directly to "C:/Documents and Settings" even if you
		//have another directory named "Documents and Settings" inside another subdirectory
		//I think Windows detects and treats special directories names like "Documents and Settings"
		//in a different manner
		//FIXME int ret = chdir(path.toUtf8().constData());
#ifdef Q_OS_WIN
		int ret = _chdir(_currentPath.toUtf8().constData());
#else
		int ret = chdir(_currentPath.toUtf8().constData());
#endif	//Q_OS_WIN

		if (ret < 0) {
			qCritical() << __FUNCTION__ << "Error: chdir() failed";
			perror(_currentPath.toUtf8().constData());
		}

#ifdef Q_OS_WIN
		char * currentDir = _getcwd(NULL, MAX_PATH);
#else
		char * currentDir = getcwd(NULL, MAX_PATH);
#endif	//Q_OS_WIN

		if (!currentDir) {
			qCritical() << __FUNCTION__ << "Error: getcwd() failed";
			perror("");
		}
		if (_currentPath != QString(currentDir).replace("\\", "/")) {
			qCritical() << __FUNCTION__ << "Error: _currentPath and currentDir are different, currentDir:"
				<< currentDir << "_currentPath:" << _currentPath;
		}

		struct dirent * entry = NULL;
		while (entry = readdir(dir)) {
			QString name(entry->d_name);

			//Avoid '.', '..' and other hidden files
			if (!name.startsWith('.')) {

				QString filename(_currentPath + '/' + name);

				if (TkFile::isDir(filename)) {
					//Filter directory matching the given pattern
					if (dirMatches(name)) {
						_files << filename;
					}

					findAllFilesInternal(name);
				}

				else {
					//Filter file matching the given pattern and extensions
					if (fileMatches(name)) {
						_files << filename;
					}

					if (_files.size() > _filesFoundLimit) {
						//Emits the signal every _filesFoundLimit files found
						emit filesFound(_files);
						_files.clear();
					}
				}
			}
		}

		QString tmp('/' + path);
		int pos = _currentPath.lastIndexOf(tmp);
		_currentPath.remove(pos, tmp.length());

		const char * parent = "..";
#ifdef Q_OS_WIN
		ret = _chdir(parent);
#else
		ret = chdir(parent);
#endif	//Q_OS_WIN

		if (ret < 0) {
			qCritical() << __FUNCTION__ << "Error: chdir() failed";
			perror(parent);
		}

		ret = closedir(dir);
		if (ret < 0) {
			qCritical() << __FUNCTION__ << "Error: closedir() failed";
			perror(path.toUtf8().constData());
		}
	}
}

bool FindFiles::dirMatches(const QString & filename) const {
	bool tmp = true;

	if (!_pattern.isEmpty()) {
		if (!filename.contains(_pattern)) {
			tmp = false;
		}
	}

	return tmp;
}

bool FindFiles::fileMatches(const QString & filename) const {
	bool tmp = true;

	if (!_extensions.isEmpty()) {
		if (!_extensions.contains(TkFile::fileExtension(filename), Qt::CaseInsensitive)) {
			tmp = false;
		}
	}

	if (!_pattern.isEmpty()) {
		if (!filename.contains(_pattern)) {
			tmp = false;
		}
	}

	return tmp;
}
