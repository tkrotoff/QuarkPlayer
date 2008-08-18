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

#ifndef FILETYPES_H
#define FILETYPES_H

#include <filetypes/filetypes_export.h>

#include <filetypes/FileType.h>

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
	 * Gets the file extension of a given category.
	 *
	 * Example: Audio: mp3, wav, mp4...
	 * There is no . before the extension name.
	 *
	 * @param category which category of file extension to look for
	 * @return file extensions of the given category
	 */
	static QStringList extensions(FileType::Category category);

	/** @see extensions() */
	static QStringList extensions(FileType::Category category1, FileType::Category category2);

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

private:

	FileTypes();
};

#endif	//FILETYPES_H
