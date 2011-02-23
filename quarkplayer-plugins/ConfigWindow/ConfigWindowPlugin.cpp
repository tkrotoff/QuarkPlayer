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

#include "ConfigWindowPlugin.h"

#include "ConfigWindow.h"
#include "ShortcutsConfig.h"
#include "ConfigWindowLogger.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/PluginManager.h>

#include <quarkplayer-plugins/MainWindow/MainWindow.h>

#include <TkUtil/ActionCollection.h>

#include <QtGui/QtGui>

Q_EXPORT_PLUGIN2(ConfigWindow, ConfigWindowPluginFactory);

const char * ConfigWindowPluginFactory::PLUGIN_NAME = "ConfigWindow";

QStringList ConfigWindowPluginFactory::dependencies() const {
	QStringList tmp;
	tmp += MainWindowFactory::PLUGIN_NAME;
	return tmp;
}

PluginInterface * ConfigWindowPluginFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new ConfigWindowPlugin(quarkPlayer, uuid, MainWindowFactory::mainWindow());
}

ConfigWindowPlugin * ConfigWindowPluginFactory::configWindowPlugin() {
	ConfigWindowPlugin * configWindowPlugin = dynamic_cast<ConfigWindowPlugin *>(PluginManager::instance().pluginInterface(PLUGIN_NAME));
	Q_ASSERT(configWindowPlugin);
	return configWindowPlugin;
}

ConfigWindowPlugin::ConfigWindowPlugin(QuarkPlayer & quarkPlayer, const QUuid & uuid,
				QWidget * mainWindow)
	: QObject(mainWindow),
	PluginInterface(quarkPlayer, uuid) {

	_configWindow = NULL;

	loadSettings();

	connect(ActionCollection::action("CommonActions.Configure"), SIGNAL(triggered()),
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
		_configWindow = new ConfigWindow(quarkPlayer().pluginManager(), qobject_cast<QWidget *>(parent()));

		//Emits the signal just once, not each time the ConfigWindow is being showed
		emit configWindowCreated(_configWindow);
	}

	_configWindow->exec();
}

ConfigWindow * ConfigWindowPlugin::configWindow() const {
	return _configWindow;
}
