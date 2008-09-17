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

#ifndef FINDFILES_H
#define FINDFILES_H

#include <tkutil/tkutil_export.h>

#include <QtCore/QThread>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>

/**
 * Find files for a directory, recursively or not.
 *
 * Optimized and threaded version that works fine with thousands of files.
 *
 * Follows the same API as PlaylistParser class.
 *
 * @see IPlaylistParser
 * @author Tanguy Krotoff
 */
class TKUTIL_API FindFiles : public QThread {
	Q_OBJECT
public:

	FindFiles(QObject * parent = 0);

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
	 * @param pattern pattern to match
	 */
	void setPattern(const QRegExp & pattern);

	/**
	 * @param extensions file extensions to match ex: "what is love.mp3" matches "mp3" extension
	 */
	void setExtensions(const QStringList & extensions);

	/**
	 * Set if we filter directories names aswell or not.
	 */
	void setFindDirs(bool findDirs);

signals:

	/**
	 * Sends the signal every _filesFoundLimit files found.
	 *
	 * @param files list of files (full path filename)
	 */
	void filesFound(const QStringList & files);

	/**
	 * Sends the signal after all filesFound() signals, this is the last signal sent.
	 *
	 * Guaranteed to be sent only once.
	 */
	void finished(int timeElapsed);

private:

	void run();

	/**
	 * UNIX/POSIX version
	 *
	 * @param path directory where to search for files.
	 */
	void findAllFilesUNIX(const QString & path);

	/**
	 * Generic Qt version.
	 *
	 * Very slow :/
	 */
	void findAllFilesQt(const QString & path);

	/**
	 * Win32 version.
	 */
	void findAllFilesWin32(const QString & path);

	/** Filter file matching the given pattern. */
	bool patternMatches(const QString & filename) const;

	/** Filter file matching the given extensions. */
	bool extensionMatches(const QString & filename) const;

	QStringList _files;

	QString _path;

	bool _findDirs;

	QRegExp _pattern;

	QStringList _extensions;

	int _filesFoundLimit;
};

#endif	//FINDFILES_H
