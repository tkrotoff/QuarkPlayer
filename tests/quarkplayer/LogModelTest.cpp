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

#include "LogModelTest.h"

#include <quarkplayer/LogModel.h>
#include <quarkplayer/LogMessage.h>

QTEST_MAIN(LogModelTest)

void LogModelTest::initTestCase() {
}

void LogModelTest::cleanupTestCase() {
}

void LogModelTest::init() {
}

void LogModelTest::cleanup() {
}

void LogModelTest::testEmpty() {
	LogModel model(this);
	QCOMPARE(model.rowCount(), 0);
	QCOMPARE(model.columnCount(), LogModel::COLUMN_COUNT);
}

void LogModelTest::testHeaderData() {
	LogModel model(this);
	int count = model.columnCount();
	for (int i = 0; i < count; i++) {
		QVERIFY(!model.headerData(i, Qt::Horizontal).toString().isEmpty());
	}
}

void LogModelTest::testData() {
	LogMessage msg1;
	msg1.message = "msg1";
	LogMessage msg2;
	msg2.message = "msg2";
	LogMessage msg3;
	msg3.message = "msg3";

	LogModel model(this);
	model.append(msg1);
	model.append(msg2);
	model.append(msg3);

	QCOMPARE(model.rowCount(), 3);
	QCOMPARE(model.data(model.index(0, LogModel::COLUMN_MESSAGE)).toString(), QString("msg1"));
	QCOMPARE(model.data(model.index(1, LogModel::COLUMN_MESSAGE)).toString(), QString("msg2"));
	QCOMPARE(model.data(model.index(2, LogModel::COLUMN_MESSAGE)).toString(), QString("msg3"));
}

void LogModelTest::testOpenSave() {
	LogModel model(this);
	bool success = model.open(":/test1.log");
	QVERIFY(success);
	int count1 = model.rowCount();
	QVERIFY(count1 > 0);

	QTemporaryFile fileTmp;
	success = fileTmp.open();
	QVERIFY(success);
	success = model.save(fileTmp.fileName());
	QVERIFY(success);
	int count2 = model.rowCount();
	QVERIFY(count2 > 0);
	QCOMPARE(count1, count2);
	QByteArray dataTmp = fileTmp.readAll();

	QFile fileTestLog(":/test1.log");
	success = fileTestLog.open(QIODevice::ReadOnly | QIODevice::Text);
	QVERIFY(success);
	QByteArray dataTestLog = fileTestLog.readAll();
	QCOMPARE(QString(dataTestLog), QString(dataTmp));
}
