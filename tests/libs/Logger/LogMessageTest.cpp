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
#ifdef __COVERAGESCANNER__
	__coveragescanner_install("LogMessageTestExec");
	__coveragescanner_clear();
#endif
}

void LogMessageTest::cleanup() {
#ifdef __COVERAGESCANNER__
	QString test_name="unittest/";
	test_name += metaObject()->className();
	test_name += "/";
	test_name += QTest::currentTestFunction();
	__coveragescanner_testname(test_name.toLatin1());
	if (QTest::currentTestFailed()) {
		__coveragescanner_teststate("FAILED");
	} else {
		__coveragescanner_teststate("PASSED") ;
	}
	__coveragescanner_save();
	__coveragescanner_testname("");
#endif
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

	//Test successful read with the other LogMessage' constructor
	QString text2 = "QP_LOGGER \"C:\\Documents and Settings\\tkrotoff\\Desktop\\quarkplayer\\trunk\\quarkplayer-app\\main.cpp\" 64 QuarkPlayerCore main Current date and time: \"ven. 29. oct. 12:13:26 2010\" ";
	LogMessage msg2(QtDebugMsg, text2);
	QCOMPARE(msg2.type, QtDebugMsg);
	QCOMPARE(msg2.file, QString("C:\\Documents and Settings\\tkrotoff\\Desktop\\quarkplayer\\trunk\\quarkplayer-app\\main.cpp"));
	QCOMPARE(msg2.line, 64);
	QCOMPARE(msg2.module, QString("QuarkPlayerCore"));
	QCOMPARE(msg2.function, QString("main"));
	QCOMPARE(msg2.message, QString("Current date and time: \"ven. 29. oct. 12:13:26 2010\""));
	///

	//Test failing read and default LogMessage value
	QString text3 = "blabla";
	LogMessage msg3;
	QXmlStreamReader reader3(text3);
	bool success3 = msg3.read(reader3);
	QVERIFY(!success3);
	QVERIFY(msg3.isEmpty());
	QCOMPARE(msg3.time, QTime());
	QCOMPARE(msg3.type, QtDebugMsg);
	QCOMPARE(msg3.file, QString());
	QCOMPARE(msg3.line, 0);
	QCOMPARE(msg3.module, QString());
	QCOMPARE(msg3.function, QString());
	QCOMPARE(msg3.message, QString());
	///

	//Compare with empty LogMessage
	LogMessage msg4;
	QCOMPARE(msg3, msg4);
	QVERIFY(msg4.isEmpty());
	///

	//Write, then read and compare
	QString text5;
	QXmlStreamWriter writer5(&text5);
	msg1.write(writer5);
	QXmlStreamReader reader5(text5);
	LogMessage msg5(reader5);
	QCOMPARE(msg5, msg1);
	QCOMPARE(msg5.time, msg1.time);
	QCOMPARE(msg5.type, msg1.type);
	QCOMPARE(msg5.file, msg1.file);
	QCOMPARE(msg5.line, msg1.line);
	QCOMPARE(msg5.module, msg1.module);
	QCOMPARE(msg5.function, msg1.function);
	QCOMPARE(msg5.message, msg1.message);
	///
}
