/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "ZipFile.h"

#include "FindSubtitlesLogger.h"

#include <QtIOCompressor>

#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QBuffer>

ZipFile::ZipFile(const QString & fileName) {
	_fileName = fileName;
}

ZipFile::~ZipFile() {
}

ZipFile::FileData ZipFile::readFile(QFile & file) {
	//Taken from examples given with QtIOCompressor

	FileData data;

	//Zip format "local file header" fields:
	quint32 signature, crc, compSize, unCompSize;
	quint16 extractVersion, bitFlag, compMethod, modTime, modDate;
	quint16 nameLen, extraLen;

	QDataStream stream(&file);
	stream.setByteOrder(QDataStream::LittleEndian);
	stream >> signature;
	if (signature != 0x04034b50) {
		//zip local file header magic number
		return data;
	}
	stream >> extractVersion >> bitFlag >> compMethod;
	stream >> modTime >> modDate >> crc >> compSize >> unCompSize;
	stream >> nameLen >> extraLen;

	data.fileName = file.read(nameLen);
	file.read(extraLen);
	data.compMethod = compMethod;
	data.compData = file.read(compSize);

	return data;
}

QStringList ZipFile::listFiles() const {
	QStringList fileList;

	QFile file(_fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		FindSubtitlesWarning() << "Failed to open file:" << _fileName;
		return fileList;
	}

	forever {
		FileData data = readFile(file);
		if (data.fileName.isEmpty()) {
			break;
		}
		fileList += data.fileName;
	}

	return fileList;
}

ZipFile::ExtractFileError ZipFile::extract(const QString & fileName, const QString & outputFileName) const {
	FindSubtitlesDebug() << "Extract:" << fileName << outputFileName;

	QFile file(_fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		FindSubtitlesWarning() << "Failed to open file:" << _fileName;
		return ExtractFileNotFoundError;
	}

	forever {
		FileData data = readFile(file);
		if (data.fileName.isEmpty()) {
			break;
		}

		if (fileName == data.fileName) {
			QByteArray unCompData;

			if (data.compMethod == 0) {
				unCompData = data.compData;
			} else {
				QBuffer compBuf(&(data.compData));
				QtIOCompressor compressor(&compBuf);
				compressor.setStreamFormat(QtIOCompressor::RawZipFormat);
				compressor.open(QIODevice::ReadOnly);
				unCompData = compressor.readAll();
			}

			//Write the decompressed data to a file
			QFile outputFile(outputFileName);
			if (outputFile.open(QIODevice::WriteOnly)) {
				QTextStream out(&outputFile);
				out << unCompData;
				return ExtractFileNoError;
			} else {
				return ExtractFileWriteError;
			}
		}
	}

	return ExtractFileNotFoundError;
}
