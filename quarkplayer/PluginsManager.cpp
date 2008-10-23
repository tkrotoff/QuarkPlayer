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

#include "PluginsManager.h"

#include "PluginFactory.h"
#include "PluginInterface.h"
#include "PluginsConfig.h"
#include "config/Config.h"

#include <tkutil/Random.h>

#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

PluginsManager::PluginsManager() {
	_quarkPlayer = NULL;
	_allPluginsAlreadyLoaded = false;
	_plugins = PluginsConfig::instance().plugins();
}

PluginsManager::~PluginsManager() {
	deleteAllPlugins();
}

void PluginsManager::loadAllPlugins(QuarkPlayer & quarkPlayer) {
	_quarkPlayer = &quarkPlayer;

	QList<int> randomList;

	QDir pluginsDir = QDir(Config::instance().pluginsDir());
	QStringList before = pluginsDir.entryList(QDir::Files);
	QStringList pluginList = Random::randomize(before);

	foreach (QString filename, pluginList) {
		if (_plugins.contains(filename)) {
			QList<PluginData> values = _plugins.values(filename);
			for (int i = 0; i < values.size(); i++) {
				PluginData pluginData(values.at(i));
				if (pluginData.isEnabled()) {
					//Plugin already exist in database
					//This plugin was already created before and should be enabled
					loadPlugin(filename, pluginData);
				}
			}
		} else {
			//Plugin does not already exist in database
			//Let's create it for the first time
			PluginData pluginData(QUuid::createUuid(), true);
			loadPlugin(filename, pluginData);
		}
	}

	_allPluginsAlreadyLoaded = true;
	emit allPluginsLoaded();
}

bool PluginsManager::loadPlugin(const QString & filename) {
	QString pluginFilename(filename);

	if (!_plugins.contains(pluginFilename)) {
		//Check if the list does not already contain a similar name
		//which will mean that we know already this plugin
		//Example: filebrowser plugin can be named
		//libfilebrowser.dll if compiled with MinGW
		//or filebrowser.dll if compiled with MSVC
		//in this case filename should be the string "filebrowser"
		PluginData::PluginListIterator it(_plugins);
		while (it.hasNext()) {
			it.next();

			QString tmp(it.key());
			if (tmp.contains(pluginFilename, Qt::CaseInsensitive)) {
				//Ok we found a similar filename
				//Should be the plugin we are looking for
				pluginFilename = tmp;
				break;
			}
		}
	}

	bool loaded = false;
	if (_plugins.contains(pluginFilename)) {
		QList<PluginData> values = _plugins.values(pluginFilename);
		for (int i = 0; i < values.size(); i++) {
			PluginData pluginData(values.at(i));
			if (!pluginData.interface()) {
				//Take the next not loaded plugin which filename matches
				//In a way, it loads a plugin that already exist in the past
				//instead of creating a new one
				loadPlugin(pluginFilename, pluginData);
				loaded = true;
				break;
			}
		}
		if (!loaded) {
			//Plugin does not already exist in database
			//Let's create it for the first time
			PluginData pluginData(QUuid::createUuid(), true);
			loadPlugin(pluginFilename, pluginData);

			loaded = true;
		}
	} else {
		qCritical() << __FUNCTION__ << "Error: unknown plugin:" << pluginFilename;
	}

	return loaded;
}

bool PluginsManager::loadPlugin(const QString & filename, PluginData & pluginData) {
	bool loaded = false;

	QString pluginFilename(filename);

	if (!pluginData.loader()) {
		//Not already loaded
		QDir pluginsDir = QDir(Config::instance().pluginsDir());
		QString filePath(pluginsDir.absoluteFilePath(pluginFilename));

		pluginData.setLoader(new QPluginLoader(filePath));
	}

	QObject * plugin = pluginData.loader()->instance();
	if (plugin) {
		PluginFactory * factory = qobject_cast<PluginFactory *>(plugin);
		if (factory) {
			pluginData.setInterface(factory->create(*_quarkPlayer, pluginData.uuid()));

			qDebug() << __FUNCTION__ << "Plugin loaded:" << pluginFilename;
			loaded = true;
		} else {
			qCritical() << __FUNCTION__ << "Error: this is not a QuarkPlayer plugin:" << pluginFilename;
		}
	} else {
		qCritical() << __FUNCTION__ << "Error: plugin couldn't be loaded:" << pluginFilename << pluginData.loader()->errorString();
	}

	if (!loaded) {
		pluginData.deleteLoader();
	}

	changeOrAddPluginData(pluginFilename, pluginData);

	QCoreApplication::processEvents();

	return loaded;
}

void PluginsManager::deleteAllPlugins() {
	PluginData::PluginListIterator it(_plugins);
	while (it.hasNext()) {
		it.next();

		QString filename(it.key());
		PluginData pluginData(it.value());
		deletePlugin(filename, pluginData);
	}
}

bool PluginsManager::allPluginsAlreadyLoaded() const {
	return _allPluginsAlreadyLoaded;
}

bool PluginsManager::deletePlugin(const QString & filename, PluginData & pluginData) {
	bool ret = true;

	if (!pluginData.interface()) {
		//qCritical() << __FUNCTION__ << "Error: couldn't delete the plugin:" << filename;
		ret = false;
	} else {
		//Unloads the plugin
		pluginData.deleteInterface();
	}

	changeOrAddPluginData(filename, pluginData);

	//FIXME Sometimes QuarkPlayer does not quit
	//some threads are still running
	//I guess this line should do it...
	QCoreApplication::processEvents();

	//This is too dangerous: it crashes
	//pluginData.deleteLoader();

	return ret;
}

void PluginsManager::changeOrAddPluginData(const QString & filename, const PluginData & pluginData) {
	_plugins.remove(filename, pluginData);
	_plugins.insert(filename, pluginData);

	if (_allPluginsAlreadyLoaded) {
		//Optimization: save the plugin list only is loading is finished
		//this means we save only modifications made by the user
		//when dealing with the configuration panel
		PluginsConfig::instance().setPlugins(_plugins);
	}
}

PluginData PluginsManager::pluginData(const QString & filename, const QUuid & uuid) const {
	PluginData data;

	PluginData::PluginListIterator it(_plugins);
	while (it.hasNext()) {
		it.next();

		PluginData pluginData(it.value());

		if (pluginData.uuid() == uuid) {
			data = pluginData;
			break;
		}
	}

	return data;
}

PluginData::PluginList PluginsManager::plugins() const {
	return _plugins;
}
