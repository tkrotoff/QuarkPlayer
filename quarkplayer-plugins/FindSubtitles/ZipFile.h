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

#ifndef ZIPFILE_H
#define ZIPFILE_H

#include <QtCore/QString>

class QStringList;
class QFile;

/**
 * Unzip a .zip file.
 *
 * Uses QtIOCompressor from Qt Solutions Catalog
 * http://qt.nokia.com/products/appdev/add-on-products/catalog/4
 * http://qt.nokia.com/products/appdev/add-on-products/catalog/4/Utilities/qtiocompressor/
 *
 * @see QtIOCompressor
 * @author Tanguy Krotoff
 */
class ZipFile {
public:

	/**
	 * Opens a .zip file given its full path name.
	 *
	 * @param fileName archive file name (full path)
	 */
	ZipFile(const QString & fileName);

	~ZipFile();

	/**
	 * List the files contained inside the .zip file.
	 *
	 * @return files contained inside the archive
	 */
	QStringList listFiles() const;

	/** Error status when extracting a file from the archive. */
	enum ExtractFileError {

		/**
		 * No error occured while extracting the file from the archive.
		 * The file extracted has been saved on the harddrive.
		 */
		ExtractFileNoError,

		/**
		 * The file specified for being extracted has not been found inside the archive,
		 * or the .zip file couldn't be opened.
		 */
		ExtractFileNotFoundError,

		/**
		 * The file to be extracted has been found but couldn't be saved on the harddrive.
		 */
		ExtractFileWriteError
	};

	/**
	 * Extract a given file from the .zip file.
	 *
	 * @param fileName the file to extract from the archive
	 * @param outputFileName file destination
	 * @return error status
	 */
	ExtractFileError extract(const QString & fileName, const QString & outputFileName) const;

private:

	/** Code factorization. */
	struct FileData {
		QString fileName;
		quint16 compMethod;
		QByteArray compData;
	};

	/** Code factorization. */
	static FileData readFile(QFile & file);

	/** Archive file name (full path). */
	QString _fileName;
};

#endif	//ZIPFILE_H
