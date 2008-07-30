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

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

static const int FILES_FOUND_LIMIT = 100;

FindFiles::FindFiles() {
}

FindFiles::~FindFiles() {
}

void FindFiles::findFiles(const QString & path) {
	QStringList files;

	QDir dir(path);
	dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	QFileInfoList fileInfoList = dir.entryInfoList();
	int filesCount = 0;
	foreach (QFileInfo fileInfo, fileInfoList) {
		filesCount++;
		files << fileInfo.absoluteFilePath();

		if (filesCount > FILES_FOUND_LIMIT) {
			//Emits the signal every FILES_FOUND_LIMIT files found
			emit filesFound(files);
			files.clear();
			filesCount = 0;
		}
	}

	//Emits the signal for the remaining files found (< FILES_FOUND_LIMIT)
	emit filesFound(files);
}

void FindFiles::findAllFiles(const QString & path) {
	findFiles(path);

	QDir dir(path);
	dir.setFilter(QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	QFileInfoList fileInfoList = dir.entryInfoList();
	foreach (QFileInfo fileInfo, fileInfoList) {
		findAllFiles(fileInfo.filePath());
	}
}
