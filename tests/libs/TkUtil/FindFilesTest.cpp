/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "FindFilesTest.h"

#include <QtGui/QtGui>

#include <TkUtil/FindFiles.h>
#include <TkUtil/TkUtilLogger.h>

QTEST_MAIN(FindFilesTest)

void FindFilesTest::initTestCase() {
}

void FindFilesTest::cleanupTestCase() {
}

void FindFilesTest::init() {
}

void FindFilesTest::cleanup() {
}

void FindFilesTest::testUTF8() {
	FindFiles * findFiles = new FindFiles(this);
	connect(findFiles, SIGNAL(filesFound(const QStringList &)),
		SLOT(filesFound(const QStringList &)));
	connect(findFiles, SIGNAL(finished(int)),
		SLOT(finished(int)));

	QSignalSpy spyFilesFound(findFiles, SIGNAL(filesFound(const QStringList &)));
	QVERIFY(spyFilesFound.isValid());
	QSignalSpy spyFinished(findFiles, SIGNAL(finished(int)));
	QVERIFY(spyFinished.isValid());

	findFiles->setSearchPath("../../tests/libs/TkUtil/FindFilesTests");
	findFiles->start();

	//Wait a bit...
	while (spyFinished.count() == 0) {
		QTest::qWait(200);
	}

	QCOMPARE(spyFilesFound.count(), 1);
	QCOMPARE(spyFinished.count(), 1);

	/*
	findFiles->setFilesFoundLimit();
	findFiles->setPattern();
	findFiles->setExtensions();
	findFiles->setFindDirs();
	findFiles->setRecursiveSearch();
	findFiles->stop();
	*/
}

void FindFilesTest::filesFoundUTF8(const QStringList & files) {
	TkUtilDebug() << "Files found:" << files.size();

	static QTextEdit * textEdit = NULL;
	if (!textEdit) {
		textEdit = new QTextEdit(NULL);
	}
	textEdit->setText(files.join("\n"));
	textEdit->show();

	QCOMPARE(files.size(), 4);
}


void FindFilesTest::testStartStop() {
	FindFiles * findFiles = new FindFiles(this);
	connect(findFiles, SIGNAL(filesFound(const QStringList &)),
		SLOT(filesFound(const QStringList &)));
	connect(findFiles, SIGNAL(finished(int)),
		SLOT(finished(int)));

	QSignalSpy spyFilesFound(findFiles, SIGNAL(filesFound(const QStringList &)));
	QVERIFY(spyFilesFound.isValid());
	QSignalSpy spyFinished(findFiles, SIGNAL(finished(int)));
	QVERIFY(spyFinished.isValid());

	findFiles->setSearchPath("/");
	findFiles->start();

	//Waits 10s before to stop
	QTest::qWait(10000);
	findFiles->stop();

	//Wait a bit otherwise finished() signal is never catched
	QTest::qWait(200);

	QVERIFY(spyFilesFound.count() > 0);
	TkUtilDebug() << "filesFound() signals:" << spyFilesFound.count();
	QCOMPARE(spyFinished.count(), 1);
}

void FindFilesTest::finishedStartStop(int timeElapsed) {
	QVERIFY(timeElapsed > 10000);
	QVERIFY(timeElapsed < 11000);
}


void FindFilesTest::testRecursionOff() {
	FindFiles * findFiles = new FindFiles(this);
	connect(findFiles, SIGNAL(filesFound(const QStringList &)),
		SLOT(filesFound(const QStringList &)));
	connect(findFiles, SIGNAL(finished(int)),
		SLOT(finished(int)));

	QSignalSpy spyFilesFound(findFiles, SIGNAL(filesFound(const QStringList &)));
	QVERIFY(spyFilesFound.isValid());
	QSignalSpy spyFinished(findFiles, SIGNAL(finished(int)));
	QVERIFY(spyFinished.isValid());

	findFiles->setSearchPath("../../tests/libs/TkUtil/FindFilesTests");
	findFiles->setRecursiveSearch(false);	//Default is true
	findFiles->start();

	//Wait a bit...
	while (spyFinished.count() == 0) {
		QTest::qWait(200);
	}

	QCOMPARE(spyFilesFound.count(), 0);
	QCOMPARE(spyFinished.count(), 1);
}

void FindFilesTest::filesFound(const QStringList & files) {
	TkUtilDebug() << "Files found:" << files.size();

	static QTextEdit * textEdit = NULL;
	if (!textEdit) {
		textEdit = new QTextEdit(NULL);
	}
	textEdit->setText(files.join("\n"));
	textEdit->show();
}

void FindFilesTest::finished(int timeElapsed) {
	TkUtilDebug() << timeElapsed;
}
