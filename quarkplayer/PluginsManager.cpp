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

PluginData::PluginList PluginsManager::pluginDataList(const QString & fileName) const {
	PluginData::PluginList pluginDataList;
	foreach(PluginData pluginData, _plugins) {
		if (pluginData.fileName() == fileName) {
			pluginDataList += pluginData;
		}
	}
	return pluginDataList;
}

QString PluginsManager::findPluginDir() const {
	QStringList pluginDirList = Config::instance().pluginDirList();
	QString appDir = QCoreApplication::applicationDirPath();

	QString tmp;
	foreach (QString pluginDir, pluginDirList) {
		QDir dir(pluginDir);

		//Order matters!!! See Config.cpp
		//Check if we have:
		//applicationDirPath/quarkplayer.exe
		//applicationDirPath/plugins/*.dll
		//Then applicationDirPath/plugins/ is the plugin directory
		//
		//Check if we have:
		//applicationDirPath/quarkplayer.exe
		//usr/lib/quarkplayer/plugins/*.dll

		qDebug() << "Checking for plugins:" << pluginDir;

		if (pluginDir.contains(appDir)) {
			if (dir.exists() && !dir.entryList(QDir::Files).isEmpty()) {
				//We have found the plugin directory
				tmp = pluginDir;
				break;
			}
		} else {
			if (dir.exists() && !dir.entryList(QDir::Files).isEmpty()) {
				//We have found the plugin directory
				tmp = pluginDir;
				break;
			}
		}
	}

	if (tmp.isEmpty()) {
		qCritical() << __FUNCTION__ << "Error: couldn't find the plugin directory";
	} else {
		qDebug() << __FUNCTION__ << "Plugin directory:" << tmp;
	}

	return tmp;
}

void PluginsManager::loadAllPlugins(QuarkPlayer & quarkPlayer) {
	//Stupid hack, see loadPlugin()
	_quarkPlayer = &quarkPlayer;
	///

	_pluginDir = findPluginDir();
	QStringList fileNameListTmp = QDir(_pluginDir).entryList(QDir::Files);

	//Removes duplicated items from the list of plugins
	PluginData::PluginList newPluginList;
	foreach(PluginData pluginData, _plugins) {
		if (fileNameListTmp.contains(pluginData.fileName())) {
			//Let's keep it
			newPluginList += pluginData;
		}
	}
	_plugins = newPluginList;
	///

	//Randomizes the list of plugins, this allows
	//to easily detect crashes/bugs
	QStringList fileNameList = Random::randomize(fileNameListTmp);

	foreach (QString fileName, fileNameList) {
		PluginData::PluginList list = pluginDataList(fileName);
		if (!list.isEmpty()) {
			foreach (PluginData pluginData, list) {
				if (pluginData.isEnabled()) {
					//This plugin is enabled => let's load it
					loadPlugin(pluginData);
				} else {
					//This plugin is disabled => don't load it
				}
			}
		} else {
			//No plugin matching the given filename in database
			//Let's create it for the first time
			PluginData pluginData(fileName, QUuid::createUuid(), true);
			loadPlugin(pluginData);
		}
	}

	_allPluginsAlreadyLoaded = true;
	emit allPluginsLoaded();
}

bool PluginsManager::loadDisabledPlugin(const PluginData & pluginData) {
	bool loaded = false;
	PluginData::PluginList list = pluginDataList(pluginData.fileName());
	if (!list.isEmpty()) {
		foreach (PluginData data, list) {
			if (!data.isEnabled()) {
				//Loads a plugin that already exist in the past
				//instead of creating a new one
				loadPlugin(data);
				loaded = true;
				break;
			}
		}
	}

	if (!loaded) {
		//Plugin does not already exist in database
		//Let's create it for the first time
		PluginData newPluginData(pluginData.fileName(), QUuid::createUuid(), true);
		loadPlugin(newPluginData);
		loaded = true;
	}

	return loaded;
}

bool PluginsManager::loadPlugin(PluginData & pluginData) {
	bool loaded = false;

	pluginData.setEnabled(true);

	if (!pluginData.loader()) {
		//Not already loaded
		pluginData.setLoader(new QPluginLoader(_pluginDir + QDir::separator() + pluginData.fileName()));
	}

	QObject * plugin = pluginData.loader()->instance();
	if (plugin) {
		PluginFactory * factory = qobject_cast<PluginFactory *>(plugin);
		if (factory) {
			pluginData.setInterface(factory->create(*_quarkPlayer, pluginData.uuid()));

			qDebug() << __FUNCTION__ << "Plugin loaded:" << pluginData.fileName();
			loaded = true;
		} else {
			qCritical() << __FUNCTION__ << "Error: this is not a QuarkPlayer plugin:" << pluginData.fileName();
		}
	} else {
		qCritical() << __FUNCTION__ << "Error: plugin couldn't be loaded:" << pluginData.fileName() << pluginData.loader()->errorString();
	}

	if (!loaded) {
		pluginData.deleteLoader();
	}

	updatePluginData(pluginData);

	QCoreApplication::processEvents();

	return loaded;
}

void PluginsManager::deleteAllPlugins() {
	foreach (PluginData pluginData, _plugins) {
		deletePlugin(pluginData);
	}
}

bool PluginsManager::allPluginsAlreadyLoaded() const {
	return _allPluginsAlreadyLoaded;
}

bool PluginsManager::deletePlugin(PluginData & pluginData) {
	bool ret = true;

	if (!pluginData.interface()) {
		//qCritical() << __FUNCTION__ << "Error: couldn't delete the plugin:" << filename;
		ret = false;
	} else {
		//Unloads the plugin
		pluginData.deleteInterface();
	}

	updatePluginData(pluginData);

	//FIXME Sometimes QuarkPlayer does not quit
	//some threads are still running
	//I guess this line should do it...
	QCoreApplication::processEvents();

	//This is too dangerous: it crashes
	//pluginData.deleteLoader();

	return ret;
}

void PluginsManager::updatePluginData(const PluginData & pluginData) {
	_plugins.removeAll(pluginData);
	_plugins += pluginData;

	if (_allPluginsAlreadyLoaded) {
		//Optimization: save the plugin list only if loading is finished
		//this means we save only modifications made by the user
		//when dealing with the configuration panel
		PluginsConfig::instance().setPlugins(_plugins);
	}
}

PluginData PluginsManager::pluginData(const QUuid & uuid) const {
	PluginData data;

	foreach (PluginData pluginData, _plugins) {
		if (pluginData.uuid() == uuid) {
			data = pluginData;
			break;
		}
	}

	if (data.uuid().isNull()) {
		qCritical() << "Error: wrong uuid:" << uuid;
	}

	//FIXME test if several identical uuid exist.
	//In this case make an assert since
	//there can't be several identical uuid
	//inside the list of plugins

	return data;
}

PluginData::PluginList PluginsManager::plugins() const {
	return _plugins;
}
