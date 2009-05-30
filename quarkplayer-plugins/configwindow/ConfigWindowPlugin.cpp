/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "ConfigWindowPlugin.h"

#include "ConfigWindow.h"
#include "ShortcutsConfig.h"

#include <quarkplayer/PluginManager.h>

#include <quarkplayer-plugins/mainwindow/MainWindow.h>

#include <tkutil/ActionCollection.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(configwindow, ConfigWindowPluginFactory);

QString ConfigWindowPluginFactory::pluginName() const {
	return "configwindow";
}

QStringList ConfigWindowPluginFactory::dependencies() const {
	QStringList tmp;
	tmp += "mainwindow";
	return tmp;
}

PluginInterface * ConfigWindowPluginFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new ConfigWindowPlugin(quarkPlayer, uuid);
}

static MainWindow * getMainWindow() {
	return dynamic_cast<MainWindow *>(PluginManager::instance().pluginInterface("mainwindow"));
}


ConfigWindowPlugin::ConfigWindowPlugin(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: PluginInterface(quarkPlayer, uuid) {

	loadSettings();

	_configWindow = NULL;

	connect(ActionCollection::action("MainWindow.Configure"), SIGNAL(triggered()),
		SLOT(showConfigWindow()));
}

ConfigWindowPlugin::~ConfigWindowPlugin() {
	saveSettings();
}

void ConfigWindowPlugin::loadSettings() const {
	ShortcutsConfig::instance().load();
}

void ConfigWindowPlugin::saveSettings() const {
	ShortcutsConfig::instance().save();
}

void ConfigWindowPlugin::showConfigWindow() {
	if (!_configWindow) {
		_configWindow = new ConfigWindow(getMainWindow());

		//Emits the signal just once, not each time the ConfigWindow is being showed
		emit configWindowCreated(_configWindow);
	}

	_configWindow->show();
}

ConfigWindow * ConfigWindowPlugin::configWindow() const {
	return _configWindow;
}
