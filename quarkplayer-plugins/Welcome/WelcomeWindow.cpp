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

#include "WelcomeWindow.h"

#include "WelcomeLogger.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/version.h>
#include <quarkplayer/PluginManager.h>

#include <quarkplayer-plugins/MainWindow/MainWindow.h>

#include <phonon/mediasource.h>

#include <QtGui/QtGui>

Q_EXPORT_PLUGIN2(Welcome, WelcomeWindowFactory);

const char * WelcomeWindowFactory::PLUGIN_NAME = "Welcome";

QStringList WelcomeWindowFactory::dependencies() const {
	QStringList tmp;
	tmp += MainWindowFactory::PLUGIN_NAME;
	return tmp;
}

PluginInterface * WelcomeWindowFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new WelcomeWindow(quarkPlayer, uuid, MainWindowFactory::mainWindow());
}

WelcomeWindow::WelcomeWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid, QWidget * mainWindow)
	: QObject(mainWindow),
	PluginInterface(quarkPlayer, uuid) {

	WelcomeDebug() << "Welcome plugin created";

	if (quarkPlayer.currentMediaObject()) {
		//There is already a Phonon::MediaObject
		//so we can start playing the webradio
		//If there is no Phonon::MediaObject, no media can be played
		playWebRadio();
	} else {
		//There is no Phonon::MediaObject, let's connect
		//to the signal that tells us when a Phonon::MediaObject is created
		connect(&quarkPlayer, SIGNAL(mediaObjectAdded(Phonon::MediaObject *)), SLOT(playWebRadio()));
	}

	QMessageBox * msgBox = new QMessageBox(mainWindow);
	connect(msgBox, SIGNAL(finished(int)), SLOT(quitPlugin()));
	msgBox->setWindowTitle(tr("Welcome!"));
	msgBox->setIcon(QMessageBox::Information);
	msgBox->setText(
		tr("Welcome to QuarkPlayer-%1!<br><br>"
		"QuarkPlayer is a multimedia application that plays your music and videos.<br><br>"
		"This is free software licensed under GNU GPLv3+ terms.<br><br>"
		"QuarkPlayer can use different backends (DirectShow, MPlayer, Xine, GStreamer, "
		"VLC...) thanks to Qt and the Phonon library. "
		"It also relies on an advanced plugin system and is available under "
		"Windows, Linux and Mac OS X.<br><br><br>Version: %2").arg(QUARKPLAYER_VERSION).arg(quarkPlayerFullVersion())
	);
	msgBox->show();
}

WelcomeWindow::~WelcomeWindow() {
	WelcomeDebug() << "Welcome plugin destroyed";
}

void WelcomeWindow::quitPlugin() {
	//Unloads and disables the plugin
	//We don't want the welcome plugin at every QuarkPlayer start
	//just at the very first start
	PluginData pluginData = quarkPlayer().pluginManager().pluginData(uuid());
	pluginData.setEnabled(false);
	quarkPlayer().pluginManager().deletePlugin(pluginData);
}

void WelcomeWindow::playWebRadio() {
	//Disconnect from the signal: we want to play the webradio only when the first Phonon::MediaObject is created
	//Warning: several Phonon::MediaObject can be created inside QuarkPlayer
	quarkPlayer().disconnect(this, SLOT(playWebRadio()));

	//Play a default webradio Live9
	//Same one as in SMPlayer, see http://www.live9.fr/
	//http://acdc2.live9.fr => IP 178.32.122.59
	quarkPlayer().play(Phonon::MediaSource("http://acdc2.live9.fr:8050"));
}
