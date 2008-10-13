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

#include "PluginManager.h"

#include "PluginFactory.h"
#include "PluginInterface.h"
#include "config/Config.h"

#include <tkutil/Random.h>

#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

PluginManager::PluginManager() {
	_quarkPlayer = NULL;
}

PluginManager::~PluginManager() {
	deleteAllPlugins();
}

void PluginManager::loadAllPlugins(QuarkPlayer & quarkPlayer) {
	_quarkPlayer = &quarkPlayer;

	QList<int> randomList;

	QDir pluginsDir = QDir(Config::instance().pluginsDir());
	QStringList before = pluginsDir.entryList(QDir::Files);
	QStringList pluginList = Random::randomize(before);

	foreach (QString filename, pluginList) {
		if (isPluginDisabled(filename)) {
			//Don't proceed plugins that are blacklisted
			qDebug() << __FUNCTION__ << "Plugin disabled:" << filename;
		} else {
			loadPlugin(filename);
		}
	}

	emit allPluginsLoaded();
}

void PluginManager::deleteAllPlugins() {
	PluginMapIterator it(_pluginMap);
	while (it.hasNext()) {
		it.next();

		deletePlugin(it.key());
	}
}

PluginManager::PluginMap & PluginManager::pluginMap() {
	return _pluginMap;
}

bool PluginManager::loadPlugin(const QString & filename) {
	bool ret = false;

	QDir pluginsDir = QDir(Config::instance().pluginsDir());
	QString filePath(pluginsDir.absoluteFilePath(filename));

	if (!QLibrary::isLibrary(filePath)) {
		//Don't proceed files that are not libraries
		qCritical() << __FUNCTION__ << "Error: this file is not a library:" << filename;
		return ret;
	}

	PluginData pluginData = _pluginMap[filename];

	if (!pluginData.loader) {
		//Not already loaded
		pluginData.loader = new QPluginLoader(filePath);
	}

	QObject * plugin = pluginData.loader->instance();
	if (plugin) {
		PluginFactory * factory = qobject_cast<PluginFactory *>(plugin);
		if (factory) {
			pluginData.interface = factory->create(*_quarkPlayer);
			qDebug() << __FUNCTION__ << "Plugin loaded:" << filename;
			ret = true;
		} else {
			qCritical() << __FUNCTION__ << "Error: this is not a QuarkPlayer plugin:" << filename;
		}
	} else {
		pluginData.loader->unload();
		delete pluginData.loader;
		pluginData.loader = NULL;
		qCritical() << __FUNCTION__ << "Error: plugin couldn't be loaded:" << filename << pluginData.loader->errorString();
	}

	_pluginMap[filename] = pluginData;

	QCoreApplication::processEvents();

	return ret;
}

bool PluginManager::deletePlugin(const QString & filename) {
	bool ret = true;

	PluginData pluginData = _pluginMap[filename];

	if (!pluginData.interface) {
		qCritical() << __FUNCTION__ << "Error: couldn't delete the plugin:" << filename;
		ret = false;
	}

	//Unloads the plugin
	delete pluginData.interface;
	pluginData.interface = NULL;

	//FIXME Sometimes QuarkPlayer does not quit
	//some threads are still running
	//I guess this line should do it...
	QCoreApplication::processEvents();

	if (pluginData.loader) {
		//This is too dangerous: it crashes
		//pluginData.loader->unload();
		//delete pluginData.loader;
		//pluginData.loader = NULL;
	}

	_pluginMap[filename] = pluginData;

	return ret;
}

void PluginManager::enablePlugin(const QString & filename) const {
	QStringList pluginsDisabled = Config::instance().pluginsDisabled();
	pluginsDisabled.removeAll(filename);
	Config::instance().setValue(Config::PLUGINS_DISABLED_KEY, pluginsDisabled);
}

void PluginManager::disablePlugin(const QString & filename) const {
	QStringList pluginsDisabled = Config::instance().pluginsDisabled();
	pluginsDisabled += filename;
	Config::instance().setValue(Config::PLUGINS_DISABLED_KEY, pluginsDisabled);
}

bool PluginManager::isPluginLoaded(const QString & filename) const {
	bool loaded = false;
	if (_pluginMap.contains(filename)) {
		PluginData pluginData = _pluginMap[filename];

		//If pluginData.interface is not NULL, this means the plugin is loaded
		loaded = pluginData.interface;
	}
	return loaded;
}

bool PluginManager::isPluginDisabled(const QString & filename) const {
	return Config::instance().pluginsDisabled().contains(filename);
}

PluginInterface * PluginManager::pluginInterface(const QString & filename) const {
	PluginInterface * interface = NULL;
	if (_pluginMap.contains(filename)) {
		PluginData pluginData = _pluginMap[filename];
		interface = pluginData.interface;
	}
	return interface;
}
