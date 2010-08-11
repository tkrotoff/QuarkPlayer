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

#ifndef FILETYPES_H
#define FILETYPES_H

#include <FileTypes/FileTypesExport.h>

#include <FileTypes/FileType.h>

#include <QtCore/QStringList>

/**
 * File types supported by QuarkPlayer.
 *
 * @see http://en.wikipedia.org/wiki/List_of_file_formats
 * @author Tanguy Krotoff
 */
class FILETYPES_API FileTypes {
public:

	/**
	 * Gets the complete list of available file types.
	 *
	 * @return list of available file types
	 */
	static QList<FileType> fileTypes();

	/**
	 * Gets the file extension of a given category.
	 *
	 * Example: Audio: mp3, wav, mp4...
	 * There is no dot before the extension name (same as in QFileInfo::suffix()
	 * and QFileInfo::completeSuffix())
	 *
	 * @param category which category of file extension to look for
	 * @return file extensions of the given category
	 */
	static QStringList extensions(FileType::Category category);

	/** @see extensions() */
	static QStringList extensions(FileType::Category category1, FileType::Category category2);

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

	/**
	 * Gets the file type given the file extension.
	 */
	static FileType fileType(const QString & extension);

	/**
	 * Gets the file type given the file type name.
	 */
	static FileType fileType(FileType::Name name);

	/**
	 * When getting the open file dialog, we need a file format filter.
	 *
	 * @see QFileDialog
	 */
	static QString toFilterFormat(const QStringList & extensions);

	/**
	 * When getting the save file dialog, we need a file format filter.
	 *
	 * @param extensions the extensions listed inside the QFileDialog
	 * @param defaultExtension the default extension that will be first in the list for QFileDialog
	 * @see QFileDialog
	 */
	static QString toSaveFilterFormat(const QStringList & extensions, const QString & defaultExtension = QString());

private:

	FileTypes();
};

#endif	//FILETYPES_H
