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

#ifndef FINDFILESTEST_H
#define FINDFILESTEST_H

#include <QtTest/QtTest>

#include <QtCore/QStringList>

/**
 * Test for FindFiles.
 *
 * @see FindFiles
 * @author Tanguy Krotoff
 */
class FindFilesTest : public QObject {
	Q_OBJECT
private slots:

	/** Called before the first testfunction is executed. */
	void initTestCase();

	/** Called after the last testfunction was executed. */
	void cleanupTestCase();

	/** Called before each testfunction is executed. */
	void init();

	/** Called after every testfunction. */
	void cleanup();


	void testUTF8();

	void testStartStop();

	void testRecursionOff();

protected slots:

	void filesFoundUTF8(const QStringList & files);

	void finishedStartStop(int timeElapsed);

	void filesFound(const QStringList & files);

	void finished(int timeElapsed);

private:
};

#endif	//FINDFILESTEST_H
