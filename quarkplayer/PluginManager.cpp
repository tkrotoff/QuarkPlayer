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

#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <cstdlib>
#include <ctime>

PluginManager * PluginManager::_pluginManager = NULL;

PluginManager::PluginManager() {
	srand(time(NULL));
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
	QList<int> randomList;

	QDir pluginsDir = QDir(QCoreApplication::applicationDirPath() + "/plugins");
	QStringList before = pluginsDir.entryList(QDir::Files);
	qDebug() << __FUNCTION__ << "Before randomize:" << before;
	QStringList pluginList = randomize(before);
	qDebug() << __FUNCTION__ << "After randomize:" << pluginList;

	foreach (QString filename, pluginList) {
		QPluginLoader loader(pluginsDir.absoluteFilePath(filename));
		QObject * plugin = loader.instance();
		if (plugin) {
			PluginFactory * factory = qobject_cast<PluginFactory *>(plugin);
			if (factory) {
				PluginInterface * interface = factory->create(quarkPlayer);
				qDebug() << __FUNCTION__ << "Plugin loaded:" << filename;
			}
		} else {
			qCritical() << __FUNCTION__ << "Error: plugin not loaded:" << filename << loader.errorString();
		}
	}
	emit allPluginsLoaded();
}

template <class T>
QList<T> & PluginManager::randomize(QList<T> & list) {
	int min = 0;
	int max = list.size();

	int randomIndex1 = randomInt(min, max - 1);
	int randomIndex2 = randomInt(min, max - 1);

	for (int i = 0; i < max; i++) {
		list.swap(randomIndex1, randomIndex2);
	}

	return list;
}

int PluginManager::randomInt(int min, int max) {
	int number = 0;

	if (min > max) {
		number = max + (int) (rand() * (min - max + 1) / (RAND_MAX + 1.0));
	} else {
		number = min + (int) (rand() * (max - min + 1) / (RAND_MAX + 1.0));
	}

	qDebug() << __FUNCTION__ << number;

	return number;
}
