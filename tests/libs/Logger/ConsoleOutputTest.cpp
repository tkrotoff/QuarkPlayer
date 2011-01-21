/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "ConsoleOutputTest.h"

#include <Logger/ConsoleOutput.h>
#include <Logger/Logger.h>
#include <Logger/LoggerLogger.h>

#include <QtCore/QtCore>

QTEST_MAIN(ConsoleOutputTest)

void ConsoleOutputTest::initTestCase() {
}

void ConsoleOutputTest::cleanupTestCase() {
}

void ConsoleOutputTest::init() {
}

void ConsoleOutputTest::cleanup() {
}


static const int NB_THREAD = 10;
static const int NB_MESSAGE_PER_THREAD = 100;

int nbMessageDisplayed = 0;
QMutex mutex;

class MyThread : public QThread {
public:

	MyThread(const QString & name) {
		_name = name;
	}

	void run() {
		for (int i = 0; i < NB_MESSAGE_PER_THREAD; i++) {
			LoggerDebug() << _name << i << QThread::currentThreadId();

			mutex.lock();
			nbMessageDisplayed++;
			mutex.unlock();
		}

		if (nbMessageDisplayed >= NB_THREAD * NB_MESSAGE_PER_THREAD) {
			QCoreApplication::quit();
		}
	}

private:

	QString _name;
};

void ConsoleOutputTest::testThreads() {
	qInstallMsgHandler(Logger::myMessageOutput);

	for (int i = 0; i < NB_THREAD; i++) {
		MyThread * thread = new MyThread("thread" + QString::number(i));
		thread->start();
	}

	QApplication::exec();
}
