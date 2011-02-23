/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2010-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef IPLUGINMANAGER_H
#define IPLUGINMANAGER_H

#include <quarkplayer/QuarkPlayerExport.h>

#include <QtCore/QObject>

class QuarkPlayer;
class PluginData;
class PluginInterface;

/**
 * Interface for PluginManager.
 *
 * IoC would have been nice here
 * This is useful for unit test
 *
 * @see PluginManager
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API IPluginManager : public QObject {
	Q_OBJECT
public:

	/** Loads all the available plugins. */
	virtual void loadAllPlugins(QuarkPlayer & quarkPlayer) = 0;

	/** Loads a given plugin. */
	virtual bool loadPlugin(PluginData & pluginData) = 0;

	/**
	 * Loads/reloads a previously disabled plugin that matches the given plugin fileName.
	 *
	 * If no previously disabled plugin exists then a new one will be created.
	 *
	 * <pre>
	 * pluginManager.loadDisabledPlugin("name_of_my_plugin_without_file_extension");
	 * </pre>
	 *
	 * @param fileName plugin to reload
	 */
	virtual bool loadDisabledPlugin(const QString & fileName) = 0;

	/**
	 * Deletes/unloads a given plugin and saves the plugins configuration.
	 *
	 * This will delete/unload all the plugins depending on the plugin asked to be deleted/unloaded.
	 */
	virtual bool deletePlugin(PluginData & pluginData) = 0;

	/** Gets the first loaded plugin interface matching the given fileName. */
	virtual PluginInterface * pluginInterface(const QString & fileName) const = 0;

signals:

	/**
	 * All plugins have been loaded.
	 *
	 * This signal is usefull for doing some processing after the GUI has
	 * been drew (i.e all plugins loaded since most of the plugins contain GUIs).
	 *
	 * Typical case:
	 * the playlist plugin loads automatically current_playlist.m3u which can
	 * be huge. Instead of doing it inside the playlist plugin constructor,
	 * we do it inside a slot connected to this signal. This way, the GUI
	 * is drew very quickly.
	 *
	 * Consider using Qt::QueuedConnection when you connect to this signal,
	 * this way you won't block the signal from being sent to other slots.
	 *
	 * Qt::AutoConnection is in fact Qt::DirectConnection
	 * and Qt::DirectConnection waits for the current slot to process before
	 * to deliver the signal to other slots.
	 *
	 * So if your slot needs a lot of time to process, it is better to use
	 * Qt::QueuedConnection
	 *
	 * @see http://doc.trolltech.com/main-snapshot/qt.html#ConnectionType-enum
	 */
	void allPluginsLoaded();
};

#endif	//IPLUGINMANAGER_H
