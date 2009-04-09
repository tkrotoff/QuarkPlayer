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

#include "FindFiles.h"

#include "TkFile.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTime>
#include <QtCore/QDebug>

#ifdef Q_OS_WIN
	#include <windows.h>
#else
	#include <dirent.h>
#endif	//Q_OS_WIN

static const int DEFAULT_FILES_FOUND_LIMIT = 500;

FindFiles::FindFiles(QObject * parent)
	: QThread(parent) {

	_findDirs = false;
	_filesFoundLimit = DEFAULT_FILES_FOUND_LIMIT;
	_stop = false;
}

FindFiles::~FindFiles() {
	stop();
}

void FindFiles::setSearchPath(const QString & path) {
	_path = path;
}

void FindFiles::setFilesFoundLimit(int filesFoundLimit) {
	_filesFoundLimit = filesFoundLimit;
}

void FindFiles::setPattern(const QRegExp & pattern) {
	_pattern = pattern;
}

void FindFiles::setExtensions(const QStringList & extensions) {
	_extensions = extensions;
}

void FindFiles::setFindDirs(bool findDirs) {
	_findDirs = findDirs;
}

void FindFiles::stop() {
	_stop = true;
	wait();
}

void FindFiles::run() {
	_stop = false;

	if (_path.isEmpty()) {
		qCritical() << __FUNCTION__ << "Error: empty path";
		return;
	}

	QTime timeElapsed;
	timeElapsed.start();

#ifdef Q_OS_WIN
	findAllFilesWin32(_path);
#else
	findAllFilesUNIX(_path);
#endif	//Q_OS_WIN
	//findAllFilesQt(_path);

	if (!_stop) {
		//Emits the signal for the remaining files found
		if (!_files.isEmpty()) {
			emit filesFound(_files);
			_files.clear();
		}

		//Emits the last signal
		emit finished(timeElapsed.elapsed());
	}
}

void FindFiles::findAllFilesQt(const QString & path) {
	if (_stop) {
		return;
	}

	QDir dir(path);

	//QDir::setNameFilters() is too slow :/
	dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	foreach (QString name, dir.entryList()) {
		if (_stop) {
			break;
		}

		QString filename(path + '/' + name);

		if (TkFile::isDir(filename)) {
			//Filter directory matching the given pattern
			if (_findDirs && patternMatches(name)) {
				_files << filename;
			}

			//Recurse
			findAllFilesQt(filename);
		}

		else {
			//Filter file matching the given pattern and extensions
			if (extensionMatches(name) && patternMatches(name)) {
				_files << filename;
			}

			if (_files.size() > _filesFoundLimit) {
				//Emits the signal every _filesFoundLimit files found
				if (!_stop) {
					emit filesFound(_files);
				}
				_files.clear();
			}
		}
	}
}

void FindFiles::findAllFilesWin32(const QString & path) {
#ifdef Q_OS_WIN
	//See http://msdn.microsoft.com/en-us/library/ms811896.aspx
	//See http://msdn.microsoft.com/en-us/library/aa364418.aspx
	//See http://msdn.microsoft.com/en-us/library/aa365247.aspx

	if (_stop) {
		return;
	}

	QString longPath("\\\\?\\" + path + "\\*");
	longPath = QDir::toNativeSeparators(longPath);

	WIN32_FIND_DATAW fileData;
	//LPCWSTR = wchar_t *
	//LPCSTR = char *
	//TCHAR = char or wchar_t
	//WCHAR = wchar_t

	//Get the first file
	HANDLE hList = FindFirstFileW((TCHAR *) longPath.utf16(), &fileData);
	if (hList == INVALID_HANDLE_VALUE) {
		qCritical() << __FUNCTION__ << "Error: no files found, error code:" << GetLastError();
	}

	else {
		//Traverse through the directory structure
		bool finished = false;
		while (!finished) {
			if (_stop) {
				break;
			}

			QString name(QString::fromUtf16((unsigned short *) fileData.cFileName));
			QString filename(path + '\\' + name);

			//Check if the object is a directory or not
			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

				//Avoid '.', '..' and other hidden files
				if (!name.startsWith('.')) {
					//Filter directory matching the given pattern
					if (_findDirs && patternMatches(name)) {
						_files << filename;
					}

					findAllFilesWin32(filename);
				}
			}

			else {
				//Filter file matching the given pattern and extensions
				if (extensionMatches(name) && patternMatches(name)) {
					_files << filename;
				}

				if (_files.size() > _filesFoundLimit) {
					//Emits the signal every _filesFoundLimit files found
					if (!_stop) {
						emit filesFound(_files);
					}
					_files.clear();
				}
			}

			if (!FindNextFileW(hList, &fileData)) {
				if (GetLastError() == ERROR_NO_MORE_FILES) {
					finished = true;
				}
			}
		}
	}

	FindClose(hList);
#else
	Q_UNUSED(path);
#endif	//Q_OS_WIN
}

void FindFiles::findAllFilesUNIX(const QString & path) {
#ifndef Q_OS_WIN
	//http://www.commentcamarche.net/forum/affich-1699952-langage-c-recuperer-un-dir

	if (_stop) {
		return;
	}

	//Warning: opendir() is limited to PATH_MAX
	//See http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
	DIR * dir = opendir(path.toUtf8().constData());
	if (!dir) {
		qCritical() << __FUNCTION__ << "Error: opendir() failed";
		perror(path.toUtf8().constData());
	} else {
		struct dirent * entry = NULL;
		while (entry = readdir(dir)) {
			if (_stop) {
				break;
			}
			QString name(entry->d_name);

			//Avoid '.', '..' and other hidden files
			if (!name.startsWith('.')) {

				QString filename(path + '/' + name);

				if (TkFile::isDir(filename)) {
					//Filter directory matching the given pattern
					if (_findDirs && patternMatches(name)) {
						_files << filename;
					}

					//Recurse
					findAllFilesUNIX(filename);
				}

				else {
					//Filter file matching the given pattern and extensions
					if (extensionMatches(name) && patternMatches(name)) {
						_files << filename;
					}

					if (_files.size() > _filesFoundLimit) {
						//Emits the signal every _filesFoundLimit files found
						if (!_stop) {
							emit filesFound(_files);
						}
						_files.clear();
					}
				}
			}
		}

		int ret = closedir(dir);
		if (ret != 0) {
			qCritical() << __FUNCTION__ << "Error: closedir() failed";
			perror(path.toUtf8().constData());
		}
	}
#else
	Q_UNUSED(path);
#endif	//Q_OS_WIN
}

bool FindFiles::patternMatches(const QString & filename) const {
	bool tmp = false;

	if (_pattern.isEmpty()) {
		tmp = true;
	} else if (filename.contains(_pattern)) {
		tmp = true;
	}

	return tmp;
}

bool FindFiles::extensionMatches(const QString & filename) const {
	bool tmp = false;

	if (_extensions.isEmpty()) {
		tmp = true;
	} else if (_extensions.contains(TkFile::fileExtension(filename), Qt::CaseInsensitive)) {
		tmp = true;
	}

	return tmp;
}
