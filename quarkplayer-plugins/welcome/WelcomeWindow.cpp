/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "WelcomeWindow.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/version.h>
#include <quarkplayer/PluginsManager.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(welcomewindow, WelcomeWindowFactory);

PluginInterface * WelcomeWindowFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new WelcomeWindow(quarkPlayer, uuid);
}

WelcomeWindow::WelcomeWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QObject(quarkPlayer.mainWindow()),
	PluginInterface(quarkPlayer, uuid) {

	qDebug() << __FUNCTION__ << "Welcome plugin created";

	QMessageBox * msgBox = new QMessageBox(quarkPlayer.mainWindow());
	connect(msgBox, SIGNAL(finished(int)), SLOT(finished(int)));
	msgBox->setWindowTitle(tr("Welcome!"));
	msgBox->setIcon(QMessageBox::Information);
	msgBox->setText(
		tr("Welcome to QuarkPlayer-%1!<br><br>"
		"QuarkPlayer is a multimedia application that play your music and videos.<br><br>"
		"This is free software and everything is licensed under GNU GPLv3+ terms.<br><br>"
		"QuarkPlayer uses different backends (DirectShow, GStreamer, VLC, MPlayer...) "
		"thanks to the Phonon library. "
		"It also relies on an advanced plugin system and is available under "
		"Windows, Linux and soon Mac OS X.").arg(QUARKPLAYER_VERSION)
	);
	msgBox->show();
}

WelcomeWindow::~WelcomeWindow() {
	qDebug() << __FUNCTION__ << "Welcome plugin destroyed";
}

void WelcomeWindow::finished(int result) {
	//Unloads and disables the plugin
	PluginData pluginData = PluginsManager::instance().pluginData(uuid());
	pluginData.setEnabled(false);
	PluginsManager::instance().deletePlugin(pluginData);
}
