/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "LogMessageTest.h"

#include <Logger/LogMessage.h>

#include <QtCore/QtCore>

QTEST_MAIN(LogMessageTest)

void LogMessageTest::initTestCase() {
}

void LogMessageTest::cleanupTestCase() {
}

void LogMessageTest::init() {
}

void LogMessageTest::cleanup() {
}

void LogMessageTest::testReadWrite() {
	//Test successful read
	QString text1 = " \
		<LogMessage> \
			<Time>00:12:50.362</Time> \
			<Type>Debug</Type> \
			<File>main.cpp</File> \
			<Line>61</Line> \
			<Module>QuarkPlayerCore</Module> \
			<Function>main</Function> \
			<Message>Hello World!</Message> \
		</LogMessage>";
	LogMessage msg1;
	QXmlStreamReader reader1(text1);
	bool success1 = msg1.read(reader1);
	QVERIFY(success1);
	QCOMPARE(msg1.time, QTime::fromString("00:12:50.362"));
	QCOMPARE(msg1.type, QtDebugMsg);
	QCOMPARE(msg1.file, QString("main.cpp"));
	QCOMPARE(msg1.line, 61);
	QCOMPARE(msg1.module, QString("QuarkPlayerCore"));
	QCOMPARE(msg1.function, QString("main"));
	QCOMPARE(msg1.message, QString("Hello World!"));
	///

	//Test failing read and default LogMessage value
	QString text2 = "blabla";
	LogMessage msg2;
	QXmlStreamReader reader2(text2);
	bool success2 = msg2.read(reader2);
	QVERIFY(!success2);
	QVERIFY(msg2.isEmpty());
	QCOMPARE(msg2.time, QTime());
	QCOMPARE(msg2.type, QtDebugMsg);
	QCOMPARE(msg2.file, QString());
	QCOMPARE(msg2.line, 0);
	QCOMPARE(msg2.module, QString());
	QCOMPARE(msg2.function, QString());
	QCOMPARE(msg2.message, QString());
	///

	//Compare with empty LogMessage
	LogMessage msg3;
	QCOMPARE(msg2, msg3);
	QVERIFY(msg3.isEmpty());
	///

	//Write, then read and compare
	QString text4;
	QXmlStreamWriter writer4(&text4);
	msg1.write(writer4);
	LogMessage msg4;
	QXmlStreamReader reader4(text4);
	msg4.read(reader4);
	QCOMPARE(msg4, msg1);
	QCOMPARE(msg4.time, msg1.time);
	QCOMPARE(msg4.type, msg1.type);
	QCOMPARE(msg4.file, msg1.file);
	QCOMPARE(msg4.line, msg1.line);
	QCOMPARE(msg4.module, msg1.module);
	QCOMPARE(msg4.function, msg1.function);
	QCOMPARE(msg4.message, msg1.message);
	///
}
