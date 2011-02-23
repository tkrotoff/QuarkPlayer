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

#ifndef MYVIDEOWIDGETTEST_H
#define MYVIDEOWIDGETTEST_H

#include <QtCore/QObject>

namespace Phonon {
	class MediaObject;
}

class QuarkPlayer;
class IMainWindow;

/**
 * Test class for MyVideoWidget.
 *
 * @see MyVideoWidget
 * @author Tanguy Krotoff
 */
class MyVideoWidgetTest : public QObject {
	Q_OBJECT
public:

	MyVideoWidgetTest(QuarkPlayer & quarkPlayer, IMainWindow * mainWindow);

	~MyVideoWidgetTest();

private slots:

	void mediaObjectAdded(Phonon::MediaObject * mediaObject);

private:

	IMainWindow * _mainWindow;
};

#endif	//MYVIDEOWIDGETTEST_H
