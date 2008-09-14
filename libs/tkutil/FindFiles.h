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

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>

/**
 * Find files for a directory, recursively or not.
 *
 * Optimized version that works fine with thousands of files.
 *
 * Don't forget to use QCoreApplication::processEvents() between
 * 2 filesFound() signal.
 *
 * Follows the same API as PlaylistParser class.
 *
 * @see IPlaylistParser
 * @author Tanguy Krotoff
 */
class TKUTIL_API FindFiles : public QObject {
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
	 * Finds all files (recursive) given a path.
	 *
	 * @param pattern pattern to match
	 * @param extensions file extensions to match ex: "what is love.mp3" matches "mp3" extension
	 */
	void findAllFiles(const QRegExp & pattern = QRegExp(), const QStringList & extensions = QStringList());

	/**
	 * Finds all files and directories (recursive) given a path.
	 *
	 * @see findAllFiles()
	 */
	void findAllFilesAndDirs(const QRegExp & patterns = QRegExp(), const QStringList & extensions = QStringList());

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
	void finished();

private:

	/**
	 * Finds root files (non recursive) of a given directory.
	 *
	 * Algorithm taken from http://www.commentcamarche.net/forum/affich-1699952-langage-c-recuperer-un-dir
	 *
	 * @param path directory where to search for files.
	 */
	void findAllFilesInternal(const QString & path);

	/** Filter directory matching the given pattern. */
	bool dirMatches(const QString & filename) const;

	/** Filter file matching the given pattern and extensions. */
	bool fileMatches(const QString & filename) const;

	QString _path;

	QString _currentPath;

	QStringList _files;

	bool _findDirs;

	QRegExp _pattern;

	QStringList _extensions;

	int _filesFoundLimit;
};

#endif	//FINDFILES_H
