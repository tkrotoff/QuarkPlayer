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

#include "FindFiles.h"

#include "TkFile.h"
#include "TkUtilLogger.h"

#ifdef Q_WS_WIN
	#include "Win32Util.h"
#endif

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTime>

#ifdef Q_WS_WIN
	#include <windows.h>
#else
	#include <dirent.h>
#endif	//Q_WS_WIN

#include <cerrno>

static const int DEFAULT_FILES_FOUND_LIMIT = 500;
FindFiles::Backend FindFiles::_backend = FindFiles::BackendNative;

FindFiles::FindFiles(QObject * parent)
	: QThread(parent) {

	_findDirs = false;
	_recursiveSearch = true;
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

void FindFiles::setRecursiveSearch(bool recursiveSearch) {
	_recursiveSearch = recursiveSearch;
}

void FindFiles::setBackend(Backend backend) {
	_backend = backend;
}

void FindFiles::stop() {
	_stop = true;
	wait();
}

void FindFiles::run() {
	Q_ASSERT(!_path.isEmpty());

#ifdef Q_WS_WIN
	//Remove the last / under Windows
	//C:/ -> C:
	if (_path.endsWith('/') || _path.endsWith('\\')) {
		_path = _path.left(_path.length() - 1);
	}
#endif	//Q_WS_WIN

	//Converts to native separators, otherwise FindFirstFile()
	//under Windows won't work if '/' separators are found
	_path = QDir::toNativeSeparators(_path);

	_stop = false;
	_files.clear();

	QTime timeElapsed;
	timeElapsed.start();

	switch (_backend) {

	case BackendNative:
#ifdef Q_WS_WIN
		findAllFilesWin32(_path);
#else
		findAllFilesUNIX(_path);
#endif	//Q_WS_WIN
		break;

	case BackendQt:
		findAllFilesQt(_path);
		break;

	default:
		TkUtilCritical() << "Unknown backend:" + _backend;
	}

	if (!_stop) {
		//Emits the signal for the remaining files found
		if (!_files.isEmpty()) {
			emit filesFound(_files);
			_files.clear();
		}
	}

	emit finished(timeElapsed.elapsed());
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

		QString fileName;
		if (path.endsWith(QDir::separator())) {
			//Avoids / (or \) to be doubled in case path == "/"
			fileName = path + name;
		} else {
			fileName = path + QDir::separator() + name;
		}

		if (TkFile::isDir(fileName)) {
			//Filter directory matching the given pattern
			if (_findDirs && fileNameMatches(name, _pattern)) {
				_files << fileName;
			}

			if (_recursiveSearch) {
				//Recurse
				findAllFilesQt(fileName);
			}
		}

		else {
			//Filter file matching the given pattern and extensions
			if (fileExtensionMatches(name, _extensions) && fileNameMatches(name, _pattern)) {
				_files << fileName;
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
#ifdef Q_WS_WIN
	//See http://msdn.microsoft.com/en-us/library/ms811896.aspx
	//See http://msdn.microsoft.com/en-us/library/aa364418.aspx
	//See http://msdn.microsoft.com/en-us/library/aa365247.aspx

	if (_stop) {
		return;
	}

	QString longPath("\\\\?\\" + path + "\\*");

	WIN32_FIND_DATAW fileData;
	//LPCWSTR = wchar_t *
	//LPCSTR = char *
	//TCHAR = char or wchar_t
	//WCHAR = wchar_t
	//See http://tkrotoff.blogspot.com/2010/04/code-snippets-about-qstring-wchart.html

	//Get the first file
	HANDLE hList = FindFirstFile(reinterpret_cast<const wchar_t *>(longPath.utf16()), &fileData);
	if (hList == INVALID_HANDLE_VALUE) {
		TkUtilWarning() << "Path:" << path << Win32Util::GetLastErrorToString(GetLastError());
	}

	else {
		//Traverse through the directory structure
		bool finished = false;
		while (!finished) {
			if (_stop) {
				break;
			}

			QString name(QString::fromUtf16(reinterpret_cast<const unsigned short *>(fileData.cFileName)));

			QString fileName;
			if (path.endsWith(QDir::separator())) {
				//Avoids / (or \) to be doubled in case path == "/"
				fileName = path + name;
			} else {
				fileName = path + QDir::separator() + name;
			}

			//Check if the object is a directory or not
			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

				//Avoid '.', '..' and other hidden files
				if (!name.startsWith('.')) {
					//Filter directory matching the given pattern
					if (_findDirs && fileNameMatches(name, _pattern)) {
						_files << fileName;
					}

					if (_recursiveSearch) {
						//Recurse
						findAllFilesWin32(fileName);
					}
				}
			}

			else {
				//Filter file matching the given pattern and extensions
				if (fileExtensionMatches(name, _extensions) && fileNameMatches(name, _pattern)) {
					_files << fileName;
				}

				if (_files.size() > _filesFoundLimit) {
					//Emits the signal every _filesFoundLimit files found
					if (!_stop) {
						emit filesFound(_files);
					}
					_files.clear();
				}
			}

			if (!FindNextFile(hList, &fileData)) {
				if (GetLastError() == ERROR_NO_MORE_FILES) {
					finished = true;
				}
			}
		}
	}

	FindClose(hList);
#else
	Q_UNUSED(path);
#endif	//Q_WS_WIN
}

void FindFiles::findAllFilesUNIX(const QString & path) {
#ifndef Q_WS_WIN
	//See scandir() manpage for an example

	if (_stop) {
		return;
	}

	//Warning: opendir() (and I guess scandir() too) is limited to PATH_MAX
	//See http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
	//filter() callback function is to NULL, I prefer to filter "myself" the entries
	//so that this algorithm is similar to the Qt and Windows implementations.
	struct dirent ** entries;
	int nbEntries = scandir(QFile::encodeName(path), &entries, NULL, alphasort);
	if (nbEntries < 0) {
		TkUtilWarning() << "scandir() failed, path:" << path << "errno:" << strerror(errno);
	} else {
		for (int i = 0; i < nbEntries; i++) {
			if (_stop) {
				break;
			}

			struct dirent * entry = entries[i];
			QString name(QFile::decodeName(entry->d_name));
			free(entry);

			//Avoid '.', '..' and other hidden files
			if (!name.startsWith('.')) {

				QString fileName;
				if (path.endsWith(QDir::separator())) {
					//Avoids / (or \) to be doubled in case path == "/"
					fileName = path + name;
				} else {
					fileName = path + QDir::separator() + name;
				}

				if (TkFile::isDir(fileName)) {
					//Filter directory matching the given pattern
					if (_findDirs && fileNameMatches(name, _pattern)) {
						_files << fileName;
					}

					if (_recursiveSearch) {
						//Recurse
						findAllFilesUNIX(fileName);
					}
				}

				else {
					//Filter file matching the given pattern and extensions
					if (fileExtensionMatches(name, _extensions) && fileNameMatches(name, _pattern)) {
						_files << fileName;
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
		free(entries);
	}
#else
	Q_UNUSED(path);
#endif	//Q_WS_WIN
}

bool FindFiles::fileNameMatches(const QString & fileName, const QRegExp & pattern) {
	bool match = false;

	if (pattern.isEmpty()) {
		match = true;
	} else if (fileName.contains(pattern)) {
		match = true;
	}

	return match;
}

bool FindFiles::fileExtensionMatches(const QString & fileName, const QStringList & extensions) {
	bool match = false;

	if (extensions.isEmpty()) {
		match = true;
	} else {
		//QFileInfo::completeSuffix() -> archive.tar.gz -> tar.gz
		//QFileInfo::suffix() -> archive.tar.gz -> gz
		foreach (QString extension, extensions) {
			if (QFileInfo(fileName).completeSuffix().contains(extension, Qt::CaseInsensitive)) {
				match = true;
				break;
			}
		}
	}

	return match;
}
