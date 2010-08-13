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

#ifndef FINDFILES_H
#define FINDFILES_H

#include <TkUtil/TkUtilExport.h>

#include <QtCore/QThread>
#include <QtCore/QUuid>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>

/**
 * Find files for a directory recursively.
 *
 * Optimized and threaded version that works fine with thousands of files.
 * There are UNIX and Win32 specific implementations, cannot be faster than that :)
 * A Qt implementation is available aswell but is way slower.
 *
 * All implementations (i.e backends) sort files in alphabetic order.
 *
 * Follows the same API as PlaylistParser class.
 *
 * @see IPlaylistParser
 * @author Tanguy Krotoff
 */
class TKUTIL_API FindFiles : public QThread {
	Q_OBJECT
public:

	FindFiles(QObject * parent);

	~FindFiles();

	/**
	 * Sets the seach path.
	 *
	 * @param path directory where to search for files.
	 */
	void setSearchPath(const QString & path);

	/**
	 * Sets when filesFound() signal will be sent.
	 *
	 * Default value is DEFAULT_FILES_FOUND_LIMIT
	 *
	 * @param filesFoundLimit number of files found before to send filesFound() signal
	 * @see filesFound()
	 */
	void setFilesFoundLimit(int filesFoundLimit);

	/**
	 * File name pattern to match.
	 *
	 * By default the pattern is empty and thus matches every file.
	 *
	 * @param pattern pattern to match
	 */
	void setPattern(const QRegExp & pattern);

	/**
	 * Sets the file extensions to match.
	 *
	 * The unmatching file will be discarsed.
	 * By default all extensions match (empty extensions list).
	 *
	 * @param extensions file extensions to match ex: "what is love.mp3" matches "mp3" extension
	 */
	void setExtensions(const QStringList & extensions);

	/**
	 * Sets if we filter directories names aswell or not.
	 *
	 * Default value is false.
	 */
	void setFindDirs(bool findDirs);

	/**
	 * Should the search be performed recursively or not.
	 *
	 * By default recursion is on.
	 */
	void setRecursiveSearch(bool recursiveSearch);

	/**
	 * Backend used by FindFiles.
	 *
	 * A backend is an implementation for searching files.
	 */
	enum Backend {

		/**
		 * Native way to search files.
		 *
		 * Under Windows this uses FindFirstFile().
		 * Under UNIX this uses readdir() and friends.
		 */
		BackendNative,

		/**
		 * Qt way to search files, uses QDir::entryList().
		 *
		 * This backend is slower than the native one, specially under Windows.
		 * This might change with future releases of Qt.
		 */
		BackendQt
	};

	/**
	 * Sets the backend to be used to search files.
	 *
	 * Can be native backend or Qt backend.
	 * By default native backend is used.
	 */
	static void setBackend(Backend backend);

	/**
	 * Stops the thread.
	 */
	void stop();

public slots:

	void start(const QUuid & uuid);

signals:

	/**
	 * Sends the signal every _filesFoundLimit files found.
	 *
	 * @param files list of files (full path fileName)
	 * @param uuid each search has its own unique id
	 */
	void filesFound(const QStringList & files, const QUuid & uuid);

	/**
	 * Sends the signal after all filesFound() signals, this is the last signal sent.
	 *
	 * Guaranteed to be sent only once.
	 */
	void finished(int timeElapsed, const QUuid & uuid);

private:

	void run();

	/**
	 * UNIX/POSIX version
	 *
	 * Very fast implementation.
	 *
	 * @param path directory where to search for files.
	 */
	void findAllFilesUNIX(const QString & path);

	/**
	 * Generic Qt version.
	 *
	 * Very slow :/
	 * That's why findAllFilesUNIX() and findAllFilesWin32() were created.
	 */
	void findAllFilesQt(const QString & path);

	/**
	 * Win32 version.
	 *
	 * Very fast implementation.
	 */
	void findAllFilesWin32(const QString & path);

	/** Filter file matching the given pattern. */
	static bool fileNameMatches(const QString & fileName, const QRegExp & pattern);

	/**
	 * Checks is a file matches one of the given extensions.
	 *
	 * Internally use QFileInfo::completeSuffix().
	 * This function should be used with extensions().
	 * Code from FindFiles.
	 *
	 * @param fileName file to check
	 * @param extensions list of extensions (without the dot) to match
	 * @return true if file matches; false otherwise
	 * @see extensions()
	 */
	static bool fileExtensionMatches(const QString & fileName, const QStringList & extensions);

	QStringList _files;

	QString _path;

	bool _findDirs;

	/** File pattern to match. */
	QRegExp _pattern;

	/** File extensions to match. */
	QStringList _extensions;

	int _filesFoundLimit;

	bool _recursiveSearch;

	/**
	 * From http://lists.trolltech.com/qt-interest/2007-03/thread00874-0.html
	 * You may want to add volatile to your bool flag declaration
	 * so the compiler will not cache the flag result; modern compilers
	 * can easily detect that your loop code does not change the flag value,
	 * and thus emit code that will not fetch the class attribute but
	 * rather use a local copy, fetched on entry into your while loop.
	 */
	volatile bool _stop;

	QUuid _uuid;

	/**
	 * Backend to use to search files, native or Qt way.
	 */
	static Backend _backend;
};

#endif	//FINDFILES_H
