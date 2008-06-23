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

void PluginManager::loadPlugins(QuarkPlayer & quarkPlayer) {
	QDir pluginsDir = QDir(QCoreApplication::applicationDirPath() + "/plugins");
	foreach (QString filename, pluginsDir.entryList(QDir::Files)) {
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
}
