/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "PluginManager.h"

#include "PluginManager_win32.h"
#include "PluginFactory.h"
#include "PluginInterface.h"
#include "PluginConfig.h"
#include "config/Config.h"
#include "QuarkPlayerCoreLogger.h"

#include <TkUtil/Random.h>

#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

PluginManager::PluginManager() {
	_quarkPlayer = NULL;
	_allPluginsLoaded = false;
}

PluginManager::~PluginManager() {
	//Saves the plugin list
	PluginConfig::instance().setPlugins(availablePlugins());

	deleteAllPlugins();
}

PluginManager & PluginManager::instance() {
	static PluginManager instance;
	return instance;
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

		QuarkPlayerCoreDebug() << "Checking for plugins:" << pluginDir;

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
		QuarkPlayerCoreDebug() << "Back to the default plugin directory";
		tmp = Config::instance().pluginDirList().first();
	}

	QuarkPlayerCoreDebug() << "Plugin directory:" << tmp;

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
	PluginManagerWin32::setDllDirectory(reinterpret_cast<const wchar_t *>(_pluginDir.utf16()));
	///

	//List of all the available plugins
	//Dynamic plugins
	QFileInfoList fileInfoList(QDir(_pluginDir).entryInfoList(QDir::Files));
	foreach (QFileInfo fileInfo, fileInfoList) {
		//Take only the base name, i.e without the extension since this part is platform dependent
		QString baseName(fileInfo.completeBaseName());
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
	QStringList fileNames(Random::randomize(_availablePlugins));

	foreach (QString fileName, fileNames) {

		//Check if the plugin has been already loaded
		if (!(_loadedPlugins.values(fileName).isEmpty())) {
			continue;
		}

		//Compare to the configuration:
		//this will tell us if we can load the plugin or not
		PluginDataList configPlugins(PluginConfig::instance().plugins().values(fileName));
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
			//No plugin matching the given fileName inside the configuration
			//Let's create it for the first time
			PluginData pluginData(fileName, QUuid::createUuid(), true);
			loadPlugin(pluginData);
		}
	}

	_allPluginsLoaded = true;
	emit allPluginsLoaded();
}

bool PluginManager::loadDisabledPlugin(const QString & fileName) {
	bool loaded = false;

	PluginDataList disabledPlugins(_disabledPlugins.values(fileName));
	foreach (PluginData pluginData, disabledPlugins) {
		//Loads a plugin that has already existed in the past
		//instead of creating a new one
		loaded = loadPlugin(pluginData);
		break;
	}

	if (!loaded) {
		//Plugin does not already exist in the configuration, nor is a static plugin
		//Let's create it for the first time
		PluginData newPluginData(fileName, QUuid::createUuid(), true);
		loaded = loadPlugin(newPluginData);
	}

	return loaded;
}

QString PluginManager::getRealPluginFileName(const QString & fileName) {
	QString tmp(fileName);
	Q_ASSERT(!tmp.isEmpty());

	if (!QLibrary::isLibrary(tmp)) {
#ifdef Q_WS_WIN
		tmp += ".dll";
#elif defined(Q_WS_X11)
		//Add the lib prefix under UNIX
		tmp.prepend("lib");
		tmp += ".so";
#else
		QuarkPlayerCoreCritical() << "Platform not supported";
#endif
	}

	return tmp;
}

bool PluginManager::loadPlugin(PluginData & pluginData) {
	bool loaded = false;

	pluginData.setEnabled(true);

	QString fileName(pluginData.fileName());

	//Creates the factory
	//2 cases: dynamic plugin and static plugin
	bool pluginFound = false;
	PluginFactory * factory = NULL;

	//First check if it is a static plugin
	//By default we prefer to first load static plugins
	foreach (QObject * plugin, QPluginLoader::staticInstances()) {
		factory = qobject_cast<PluginFactory *>(plugin);
		if (factory) {
			if (fileName == factory->name()) {
				//Ok, this is a static plugin
				pluginData.setFactory(factory);
				pluginFound = true;
				QuarkPlayerCoreDebug() << "Loading static plugin:" << fileName << "...";
				break;
			}
		}
	}

	if (!pluginFound) {
		//The static plugin was not found, let's check for a dynamic plugin
		QPluginLoader loader(_pluginDir + QDir::separator() + getRealPluginFileName(fileName));
		QObject * plugin = loader.instance();
		if (plugin) {
			//Ok, this is a dynamic plugin
			factory = qobject_cast<PluginFactory *>(plugin);
			if (factory) {
				pluginData.setFactory(factory);
				pluginFound = true;
				QuarkPlayerCoreDebug() << "Loading dynamic plugin:" << fileName << "...";
			} else {
				QuarkPlayerCoreCritical() << "This is not a QuarkPlayer plugin:" << fileName;
			}
		} else {
			QuarkPlayerCoreCritical() << "Couldn't load the plugin:" << fileName << loader.errorString();
		}
	}
	///

	//Check plugin dependencies and load them
	//before to load the actual plugin
	if (factory) {
		QStringList dependsOn(factory->dependencies());

		foreach (QString fileNameDepend, dependsOn) {

			bool dependSolved = false;

			//Check in the list of already loaded plugins
			PluginDataList loadedPlugins = _loadedPlugins.values(fileNameDepend);
			foreach (PluginData data, loadedPlugins) {
				//If we are here, it means the dependency plugin is already loaded
				//so no problem
				dependSolved = true;
				break;
			}

			if (!dependSolved) {
				//Compare to the configuration
				PluginDataList configPlugins(PluginConfig::instance().plugins().values(fileNameDepend));
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
					if (fileNameDepend == availableFilename) {
						//Ok, the dependency plugin has been found
						//and is not loaded nor disabled
						PluginData pluginDataDepend(fileNameDepend, QUuid::createUuid(), true);
						loadPlugin(pluginDataDepend);
						dependSolved = true;
						break;
					}
				}
			}

			if (!dependSolved) {
				//What to do?
				QuarkPlayerCoreCritical() << "Missing dependency:" << fileNameDepend;
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

			QuarkPlayerCoreDebug() << "Plugin loaded:" << fileName;
			loaded = true;
		} else {
			loaded = false;
		}
	}
	///

	//Wait for the plugin to be completely loaded graphically
	//QCoreApplication::processEvents();

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
	QString fileName(pluginData.fileName());
	Q_ASSERT(!fileName.isEmpty());
	Q_ASSERT(!pluginData.uuid().isNull());

	bool ret = true;

	if (!pluginData.interface()) {
		QuarkPlayerCoreCritical() << "Couldn't delete the plugin:" << fileName;
		ret = false;
	} else {
		QuarkPlayerCoreDebug() << "Delete plugin:" << fileName << "...";

		//Recursively unloads all the plugins depending on this one
		foreach (PluginData data, _loadedPlugins) {
			PluginFactory * factory = data.factory();
			Q_ASSERT(factory);
			if (factory->dependencies().contains(fileName)) {
				//Ok, this plugin depends on the one we want to delete
				deletePluginWithoutSavingConfig(data);
			}
		}
		///

		//Unloads the plugin
		pluginData.deleteInterface();
		QuarkPlayerCoreDebug() << "Plugin deleted:" << fileName;

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

PluginInterface * PluginManager::pluginInterface(const QString & fileName) const {
	Q_ASSERT(!fileName.isEmpty());

	PluginInterface * interface = NULL;
	foreach (PluginData pluginData, _loadedPlugins) {
		if (pluginData.fileName() == fileName) {
			interface = pluginData.interface();
			break;
		}
	}

	if (!interface) {
		QuarkPlayerCoreCritical() << "Plugin not found:" << fileName;
	}

	return interface;
}

PluginDataList PluginManager::availablePlugins() const {
	PluginDataList availablePlugins;
	availablePlugins += _loadedPlugins;
	availablePlugins += _disabledPlugins;
	return availablePlugins;
}
