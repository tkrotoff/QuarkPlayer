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

#include "LogMessageHandlerTest.h"

#include <quarkplayer/LogMessageHandler.h>
#include <quarkplayer/LogModel.h>
#include <quarkplayer/LogMessage.h>
#include <quarkplayer/QuarkPlayerCoreLogger.h>

#include <QtCore/QtCore>

QTEST_MAIN(LogMessageHandlerTest)

static const int NB_THREAD = 10;
static const int NB_MESSAGE_PER_THREAD = 1000;
static int nbMessageDisplayed = 0;

class MyThread : public QThread {
public:

	MyThread(const QString & name) {
		_name = name;
	}

	void run() {
		for (int i = 0; i < NB_MESSAGE_PER_THREAD; i++) {
			QuarkPlayerCoreDebug() << _name << i << QThread::currentThreadId();
			msleep(10);
			nbMessageDisplayed++;
		}
		if (nbMessageDisplayed == NB_THREAD * NB_MESSAGE_PER_THREAD) {
			QCoreApplication::quit();
		}
	}

private:

	QString _name;
};

void LogMessageHandlerTest::initTestCase() {
}

void LogMessageHandlerTest::cleanupTestCase() {
}

void LogMessageHandlerTest::init() {
}

void LogMessageHandlerTest::cleanup() {
}

void LogMessageHandlerTest::testThreads() {
	qInstallMsgHandler(LogMessageHandler::myMessageOutput);

	for (int i = 0; i < NB_THREAD; i++) {
		MyThread * thread = new MyThread("thread" + QString::number(i));
		thread->start();
	}

	QApplication::exec();
}
