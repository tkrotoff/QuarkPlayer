/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "MyVideoWidgetTest.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MockPluginManager.h>

#include <quarkplayer-plugins/MainWindow/MockMainWindow.h>
#include <quarkplayer-plugins/VideoWidget/VideoWidgetPlugin.h>
#include <quarkplayer-plugins/VideoWidget/MyVideoWidget.h>
#include <quarkplayer-plugins/PlayToolBar/PlayToolBar.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

MyVideoWidgetTest::MyVideoWidgetTest(QuarkPlayer & quarkPlayer, IMainWindow * mainWindow) {
	_mainWindow = mainWindow;

	connect(&quarkPlayer, SIGNAL(mediaObjectAdded(Phonon::MediaObject *)),
		SLOT(mediaObjectAdded(Phonon::MediaObject *)));
}

MyVideoWidgetTest::~MyVideoWidgetTest() {
}

void MyVideoWidgetTest::mediaObjectAdded(Phonon::MediaObject * mediaObject) {
	QDockWidget * videoDockWidget = new QDockWidget();

	//videoWidget
	MyVideoWidget * videoWidget = new MyVideoWidget(videoDockWidget, _mainWindow);
	videoDockWidget->setWidget(videoWidget);
	Phonon::createPath(mediaObject, videoWidget);

	//Add to the main window
	_mainWindow->addVideoDockWidget(videoDockWidget);
}

int main(int argc, char * argv[]) {
	QApplication app(argc, argv);

	//General infos
	app.setOrganizationName("QuarkPlayer");
	app.setOrganizationDomain("quarkplayer.org");
	app.setApplicationName("QuarkPlayer");

	MockPluginManager * pluginManager = new MockPluginManager();
	QuarkPlayer quarkPlayer(pluginManager, &app);

	MockMainWindow * mainWindow = new MockMainWindow(quarkPlayer, QUuid::createUuid());

	new MyVideoWidgetTest(quarkPlayer, mainWindow);

	pluginManager->loadAllPlugins(quarkPlayer);

	return app.exec();
}
