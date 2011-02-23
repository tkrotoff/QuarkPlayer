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

#include "PlaylistWidgetTest.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MockPluginManager.h>

#include <quarkplayer-plugins/MainWindow/MockMainWindow.h>
#include <quarkplayer-plugins/Playlist/PlaylistWidget.h>

#include <Logger/Logger.h>

#include <QtGui/QtGui>

Q_IMPORT_PLUGIN(Playlist)
Q_IMPORT_PLUGIN(MainWindow)
Q_IMPORT_PLUGIN(ConfigWindow)

PlaylistWidgetTest::PlaylistWidgetTest(QuarkPlayer & quarkPlayer, IMainWindow * mainWindow) {
	new PlaylistWidget(quarkPlayer, QUuid::createUuid(), mainWindow);
}

PlaylistWidgetTest::~PlaylistWidgetTest() {
}

int main(int argc, char * argv[]) {
	QApplication app(argc, argv);

	//General infos
	app.setOrganizationName("QuarkPlayer");
	app.setOrganizationDomain("quarkplayer.org");
	app.setApplicationName("QuarkPlayer");

	//Installs a custom message handler for qDebug(), qWarning() and others
	qInstallMsgHandler(Logger::myMessageOutput);

	MockPluginManager pluginManager;
	QuarkPlayer quarkPlayer(pluginManager, &app);

	MockMainWindow * mainWindow = new MockMainWindow(quarkPlayer, QUuid::createUuid());

	new PlaylistWidgetTest(quarkPlayer, mainWindow);

	pluginManager.loadAllPlugins(quarkPlayer);

	return app.exec();
}
