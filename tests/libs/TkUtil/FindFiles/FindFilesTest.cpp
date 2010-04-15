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

#include "FindFilesTest.h"

#include <QtGui/QtGui>

#include <TkUtil/FindFiles.h>

QTEST_MAIN(FindFilesTest)

void FindFilesTest::initTestCase() {
	_findFiles = new FindFiles(this);
	connect(_findFiles, SIGNAL(filesFound(const QStringList &, const QUuid &)),
		SLOT(filesFound(const QStringList &, const QUuid &)));
	connect(_findFiles, SIGNAL(finished(int, const QUuid &)),
		SLOT(finished(int, const QUuid &)));
}

void FindFilesTest::cleanupTestCase() {
}

void FindFilesTest::test() {
	_findFiles->setSearchPath("../../tests/libs/TkUtil/FindFiles/tests");

	QSignalSpy spyFilesFound(_findFiles, SIGNAL(filesFound(const QStringList &, const QUuid &)));
	QSignalSpy spyFinished(_findFiles, SIGNAL(finished(int, const QUuid &)));

	qDebug() << Q_FUNC_INFO << spyFinished.count();

	QUuid uuid = QUuid::createUuid();
	_findFiles->start(uuid);

	qDebug() << Q_FUNC_INFO << spyFilesFound.count();
	qDebug() << Q_FUNC_INFO << spyFinished.count();

	//if (spyFinished.count() == 0) {
		QTestEventLoop::instance().enterLoop(30);
		QVERIFY(!QTestEventLoop::instance().timeout());
	//}

	/*
	_findFiles->setFilesFoundLimit();
	_findFiles->setPattern();
	_findFiles->setExtensions();
	_findFiles->setFindDirs();
	_findFiles->setRecursiveSearch();
	_findFiles->stop();
	*/
}

void FindFilesTest::filesFound(const QStringList & files, const QUuid & uuid) {
	qDebug() << Q_FUNC_INFO << files << uuid;
	QTextEdit * textEdit = new QTextEdit(NULL);
	textEdit->setText(files.join(","));
	textEdit->show();
}

void FindFilesTest::finished(int timeElapsed, const QUuid & uuid) {
	qDebug() << Q_FUNC_INFO << timeElapsed << uuid;

	//QTestEventLoop::instance().exitLoop();
}
