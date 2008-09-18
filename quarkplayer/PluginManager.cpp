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

PluginManager * PluginManager::_pluginManager = NULL;

PluginManager::PluginManager() {
}

PluginManager::~PluginManager() {
}

PluginManager & PluginManager::instance() {
	if (!_pluginManager) {
		_pluginManager = new PluginManager();
	}
	return *_pluginManager;
}

void PluginManager::loadPlugins(QuarkPlayer & quarkPlayer) {
	QCoreApplication::processEvents();

	QList<int> randomList;

	QDir pluginsDir = QDir(Config::instance().pluginsDir());
	QStringList before = pluginsDir.entryList(QDir::Files);
	QStringList pluginList = Random::randomize(before);

	foreach (QString filename, pluginList) {
		QString filePath(pluginsDir.absoluteFilePath(filename));

		if (!QLibrary::isLibrary(filePath)) {
			//Don't proceed files that are not libraries
			continue;
		}

		PluginData pluginData;
		pluginData.loader = NULL;
		pluginData.interface = NULL;
		_pluginMap[filename] = pluginData;

		if (Config::instance().pluginsDisabled().contains(filename)) {
			//This means this plugin must not be loaded
			continue;
		}

		QPluginLoader * loader = new QPluginLoader(filePath);

		if (loader) {
			QObject * plugin = loader->instance();
			pluginData.loader = loader;
			if (plugin) {
				PluginFactory * factory = qobject_cast<PluginFactory *>(plugin);
				if (factory) {
					PluginInterface * interface = factory->create(quarkPlayer);
					pluginData.interface = interface;
					qDebug() << __FUNCTION__ << "Plugin loaded:" << filename;
				}
			} else {
				loader->unload();
				qCritical() << __FUNCTION__ << "Error: plugin not loaded:" << filename << loader->errorString();
			}
		}

		_pluginMap[filename] = pluginData;

		QCoreApplication::processEvents();
	}

	emit allPluginsLoaded();
}

PluginManager::PluginMap PluginManager::pluginMap() const {
	return _pluginMap;
}

void PluginManager::deleteAllPlugins() {
	PluginMapIterator it(_pluginMap);
	while (it.hasNext()) {
		it.next();
		PluginData pluginData = it.value();
		if (pluginData.interface) {
			delete pluginData.interface;
			pluginData.interface = NULL;
		}
		if (pluginData.loader) {
			//This is too dangerous: it crashes
			//pluginData.loader->unload();
		}
		//it.value() = pluginData;
	}
}
