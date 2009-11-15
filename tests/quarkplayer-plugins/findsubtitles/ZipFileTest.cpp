/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "ZipFileTest.h"

#include <quarkplayer-plugins/findsubtitles/ZipFile.h>

#include <QtCore/QDebug>

void ZipFileTest::initTestCase() {
}

void ZipFileTest::cleanupTestCase() {
}

void ZipFileTest::init() {
}

void ZipFileTest::cleanup() {
}

void ZipFileTest::testListFiles_data() {
	QTest::addColumn<QString>("zipFileName");
	QTest::addColumn<QStringList>("zipFileList");

	QStringList zipFileList;

	zipFileList += "firstfile.txt";
	zipFileList += "secondfile.txt";
	zipFileList += "thirdfile.txt";
	QTest::newRow("archive.zip") << ":/archive.zip"
		<< zipFileList;
}

void ZipFileTest::testListFiles() {
	QFETCH(QString, zipFileName);
	ZipFile zipFile(zipFileName);

	QStringList files = zipFile.listFiles();
	QFETCH(QStringList, zipFileList);
	for (int i = 0; i < files.size(); i++) {
		QCOMPARE(files[i], zipFileList[i]);
	}
}

void ZipFileTest::testExtract_data() {
}

void ZipFileTest::testExtract() {
}
