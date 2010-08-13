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

#ifndef QDOCKWIDGETTEST_H
#define QDOCKWIDGETTEST_H

#include <QtTest/QtTest>

class QMainWindow;
class QDockWidget;

class PaintEventFilter : public QObject {
	Q_OBJECT
private:

	bool eventFilter(QObject * object, QEvent * event);
};

/**
 * Test a the QTabBar hack for QDockWidget.
 *
 * @see QDockWidget
 * @author Tanguy Krotoff
 */
class QDockWidgetTest : public QObject {
	Q_OBJECT
private slots:

	/** Called before the first testfunction is executed. */
	void initTestCase();

	/** Called after the last testfunction was executed. */
	void cleanupTestCase();

	/** Show QDockWidget GUI. */
	void showGui();

	void showQTabBarHack();

	void printHelloWorld();

private:

	QDockWidget * _dockWidget3;

	QMainWindow * _mainWindow;
};

#endif	//QDOCKWIDGETTEST_H
