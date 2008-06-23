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

QStringList FindFiles::findFiles(const QString & path) {
	QStringList files;

	QDir dir(path);
	dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	QFileInfoList fileInfoList = dir.entryInfoList();
	foreach (QFileInfo fileInfo, fileInfoList) {
		files << fileInfo.absoluteFilePath();
	}

	return files;
}

QStringList FindFiles::findAllFiles(const QString & path) {
	QStringList files;

	files << findFiles(path);

	QDir dir(path);
	dir.setFilter(QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	QFileInfoList fileInfoList = dir.entryInfoList();
	foreach (QFileInfo fileInfo, fileInfoList) {
		files << findAllFiles(fileInfo.filePath());
	}

	return files;
}
