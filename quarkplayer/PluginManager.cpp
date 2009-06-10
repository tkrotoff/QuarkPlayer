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

#include "PluginManager.h"

#include "PluginManager_win32.h"

#include "PluginFactory.h"
#include "PluginInterface.h"
#include "PluginConfig.h"
#include "config/Config.h"

#include <tkutil/Random.h>

#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

PluginManager::PluginManager() {
	_quarkPlayer = NULL;
	_allPluginsLoaded = false;
}

PluginManager::~PluginManager() {
	//Saves the plugin list
	PluginConfig::instance().setPlugins(availablePlugins());

	deleteAllPlugins();
}

QString PluginManager::findPluginDir() const {
	QStringList pluginDirList(Config::instance().pluginDirList());
	QString appDir(QCoreApplication::applicationDirPath());

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
		qCritical() << __FUNCTION__ << "Error: couldn't find the plugin directory, back to the default one";
		tmp = Config::instance().pluginDirList().first();
	}

	qDebug() << __FUNCTION__ << "Plugin directory:" << tmp;

	return tmp;
}

void PluginManager::loadAllPlugins(QuarkPlayer & quarkPlayer) {
	Q_ASSERT(!_allPluginsLoaded);

	//Stupid hack, see loadPlugin()
	_quarkPlayer = &quarkPlayer;
	///

	//Creates the list of static plugins
	QStringList staticPlugins;
	foreach (QObject * plugin, QPluginLoader::staticInstances()) {
		if (plugin) {
			PluginFactory * factory = qobject_cast<PluginFactory *>(plugin);
			if (factory) {
				staticPlugins += factory->name();
			}
		}
	}
	///

	_pluginDir = findPluginDir();

	//Windows specific code
	//Needs to be in different .cpp file otherwise it fails to compile
	PluginManagerWin32::setErrorMode();
	PluginManagerWin32::setDllDirectory(_pluginDir.toUtf8().constData());
	///

	//List of all the available plugins
	//Dynamic plugins
	QFileInfoList fileInfoList(QDir(_pluginDir).entryInfoList(QDir::Files));
	foreach (QFileInfo fileInfo, fileInfoList) {
		//Take only the base name, i.e without the extension since this part is platform dependent
		QString baseName(fileInfo.baseName());
#ifdef Q_WS_X11
		if (baseName.startsWith("lib")) {
			//Remove the "lib" prefix under UNIX
			baseName.remove(QRegExp("^lib"));
		}
#endif	//Q_WS_X11
		_availablePlugins += baseName;
	}
	//Static plugins
	_availablePlugins += staticPlugins;

	//Randomizes the list of available plugins, this allows
	//to easily detect crashes/bugs due to order processing
	QStringList filenames(Random::randomize(_availablePlugins));

	foreach (QString filename, filenames) {

		//Check if the plugin has been already loaded
		if (!(_loadedPlugins.values(filename).isEmpty())) {
			continue;
		}

		//Compare to the configuration:
		//this will tell us if we can load the plugin or not
		PluginDataList configPlugins(PluginConfig::instance().plugins().values(filename));
		foreach (PluginData pluginData, configPlugins) {
			if (pluginData.isEnabled()) {
				//This plugin is enabled => let's load it
				loadPlugin(pluginData);
			} else {
				//This plugin is disabled => don't load it
				_disabledPlugins += pluginData;
			}
		}
		///

		if (configPlugins.isEmpty()) {
			//No plugin matching the given filename inside the configuration
			//Let's create it for the first time
			PluginData pluginData(filename, QUuid::createUuid(), true);
			loadPlugin(pluginData);
		}
	}

	_allPluginsLoaded = true;
	emit allPluginsLoaded();
}

bool PluginManager::loadDisabledPlugin(const QString & filename) {
	bool loaded = false;

	PluginDataList disabledPlugins(_disabledPlugins.values(filename));
	foreach (PluginData pluginData, disabledPlugins) {
		//Loads a plugin that has already existed in the past
		//instead of creating a new one
		loaded = loadPlugin(pluginData);
		break;
	}

	if (!loaded) {
		//Plugin does not already exist in the configuration, nor is a static plugin
		//Let's create it for the first time
		PluginData newPluginData(filename, QUuid::createUuid(), true);
		loaded = loadPlugin(newPluginData);
	}

	return loaded;
}

QString PluginManager::getRealPluginFileName(const QString & filename) {
	QString tmp(filename);
	Q_ASSERT(!tmp.isEmpty());

	if (!QLibrary::isLibrary(tmp)) {
#ifdef Q_WS_WIN
		tmp += ".dll";
#elif defined(Q_WS_X11)
		//Add the lib prefix under UNIX
		tmp.prepend("lib");
		tmp += ".so";
#else
		qFatal() << __FUNCTION__ << "Platform not supported";
#endif
	}

	return tmp;
}

bool PluginManager::loadPlugin(PluginData & pluginData) {
	bool loaded = false;

	pluginData.setEnabled(true);

	QString filename(pluginData.fileName());

	//Creates the factory
	//2 cases: dynamic plugin and static plugin
	bool pluginFound = false;
	PluginFactory * factory = NULL;

	//First check if it is a static plugin
	//By default we prefer to first load static plugins
	foreach (QObject * plugin, QPluginLoader::staticInstances()) {
		factory = qobject_cast<PluginFactory *>(plugin);
		if (factory) {
			if (filename == factory->name()) {
				//Ok, this is a static plugin
				pluginData.setFactory(factory);
				pluginFound = true;
				qDebug() << __FUNCTION__ << "Loading static plugin:" << filename << "...";
				break;
			}
		}
	}

	if (!pluginFound) {
		//The static plugin was not found, let's check for a dynamic plugin
		QPluginLoader loader(_pluginDir + QDir::separator() + getRealPluginFileName(filename));
		QObject * plugin = loader.instance();
		if (plugin) {
			//Ok, this is a dynamic plugin
			factory = qobject_cast<PluginFactory *>(plugin);
			if (factory) {
				pluginData.setFactory(factory);
				pluginFound = true;
				qDebug() << __FUNCTION__ << "Loading dynamic plugin:" << filename << "...";
			} else {
				qCritical() << __FUNCTION__ << "Error: this is not a QuarkPlayer plugin:" << filename;
			}
		} else {
			qCritical() << __FUNCTION__ << "Error: plugin couldn't be loaded:" << filename << loader.errorString();
		}
	}
	///

	//Check plugin dependencies and load them
	//before to load the actual plugin
	if (factory) {
		QStringList dependsOn(factory->dependencies());

		foreach (QString filenameDepend, dependsOn) {

			bool dependSolved = false;

			//Check in the list of already loaded plugins
			PluginDataList loadedPlugins = _loadedPlugins.values(filenameDepend);
			foreach (PluginData data, loadedPlugins) {
				//If we are here, it means the dependency plugin is already loaded
				//so no problem
				dependSolved = true;
				break;
			}

			if (!dependSolved) {
				//Compare to the configuration
				PluginDataList configPlugins(PluginConfig::instance().plugins().values(filenameDepend));
				foreach (PluginData data, configPlugins) {
					if (data.isEnabled()) {
					} else {
						//FIXME load the disabled plugin?
					}
					loadPlugin(data);
					dependSolved = true;
					break;
				}
			}

			if (!dependSolved) {
				//Still not found the dependency plugin?
				//Check in the list of available plugins
				//these plugins are neither part of the two categories at the moment:
				//loaded or disabled
				foreach (QString availableFilename, _availablePlugins) {
					if (filenameDepend == availableFilename) {
						//Ok, the dependency plugin has been found
						//and is not loaded nor disabled
						PluginData pluginDataDepend(filenameDepend, QUuid::createUuid(), true);
						loadPlugin(pluginDataDepend);
						dependSolved = true;
						break;
					}
				}
			}

			if (!dependSolved) {
				//What to do?
				qCritical() << __FUNCTION__ << "Missing dependency:" << filenameDepend;
			}
		}
	}
	///


	//Use the PluginFactory and create the PluginInterface
	if (pluginData.interface()) {
		//Plugin already loaded
		loaded = true;
	} else {
		factory = pluginData.factory();
		if (factory) {
			PluginInterface * interface = factory->create(*_quarkPlayer, pluginData.uuid());
			pluginData.setInterface(interface);

			//Add the loaded plugin to the list of loaded plugins
			//"So the last shall be first, and the first last" Matthew 20:16 ;-)
			//We need to delete the plugins backward: delete first the last loaded plugin
			//This avoid to delete the MainWindow first and then the plugins that depend on it => crash
			//This is why we prepend the loaded plugins to the list of loaded plugins
			_loadedPlugins.prepend(pluginData);

			qDebug() << __FUNCTION__ << "Plugin loaded:" << filename;
			loaded = true;
		} else {
			loaded = false;
		}
	}
	///

	QCoreApplication::processEvents();

	return loaded;
}

void PluginManager::deleteAllPlugins() {
	foreach (PluginData pluginData, _loadedPlugins) {
		deletePluginWithoutSavingConfig(pluginData);
	}
}

bool PluginManager::allPluginsAlreadyLoaded() const {
	return _allPluginsLoaded;
}

bool PluginManager::deletePlugin(PluginData & pluginData) {
	bool ret = deletePluginWithoutSavingConfig(pluginData);
	if (ret) {
		//Update and saves the plugins configuration
		PluginConfig::instance().setPlugins(availablePlugins());
	}
	return ret;
}

bool PluginManager::deletePluginWithoutSavingConfig(PluginData & pluginData) {
	QString filename(pluginData.fileName());
	Q_ASSERT(!filename.isEmpty());
	Q_ASSERT(!pluginData.uuid().isNull());

	bool ret = true;

	if (!pluginData.interface()) {
		qCritical() << __FUNCTION__ << "Error: couldn't delete the plugin:" << filename;
		ret = false;
	} else {
		qDebug() << __FUNCTION__ << "Delete plugin:" << filename << "...";

		//Recursively unloads all the plugins depending on this one
		foreach (PluginData data, _loadedPlugins) {
			PluginFactory * factory = data.factory();
			Q_ASSERT(factory);
			if (factory->dependencies().contains(filename)) {
				//Ok, this plugin depends on the one we want to delete
				deletePluginWithoutSavingConfig(data);
			}
		}
		///

		//Unloads the plugin
		pluginData.deleteInterface();
		qDebug() << __FUNCTION__ << "Plugin deleted:" << filename;

		//Update the list of loaded plugins
		//based on pluginData.uuid()
		_loadedPlugins.removeAll(pluginData);
		_disabledPlugins.append(pluginData);
		///
	}

	//FIXME Sometimes QuarkPlayer does not quit
	//some threads are still running
	//I guess this line should do it...
	QCoreApplication::processEvents();

	return ret;
}

PluginData PluginManager::pluginData(const QUuid & uuid) const {
	Q_ASSERT(!uuid.isNull());

	PluginData data;

	foreach (PluginData pluginData, availablePlugins()) {
		if (pluginData.uuid() == uuid) {
			data = pluginData;
			break;
		}
	}

	//FIXME test if several identical uuid exist.
	//In this case make an assert since
	//there can't be several identical uuid
	//inside the list of loaded plugins

	return data;
}

PluginInterface * PluginManager::pluginInterface(const QString & filename) const {
	Q_ASSERT(!filename.isEmpty());

	PluginInterface * interface = NULL;
	foreach (PluginData pluginData, _loadedPlugins) {
		if (pluginData.fileName() == filename) {
			interface = pluginData.interface();
			break;
		}
	}
	return interface;
}

PluginDataList PluginManager::availablePlugins() const {
	PluginDataList availablePlugins;
	availablePlugins += _loadedPlugins;
	availablePlugins += _disabledPlugins;
	return availablePlugins;
}
