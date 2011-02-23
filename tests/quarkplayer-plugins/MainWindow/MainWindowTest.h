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

#ifndef MAINWINDOWTEST_H
#define MAINWINDOWTEST_H

#include <QtCore/QObject>

class QuarkPlayer;

/**
 * Test class for MainWindow.
 *
 * @see MainWindow
 * @author Tanguy Krotoff
 */
class MainWindowTest : public QObject {
	Q_OBJECT
public:

	MainWindowTest(QuarkPlayer & quarkPlayer);

	~MainWindowTest();

private:
};

#endif	//MAINWINDOWTEST_H
