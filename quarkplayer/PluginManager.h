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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <quarkplayer/quarkplayer_export.h>
#include <quarkplayer/PluginData.h>

#include <tkutil/Singleton.h>

#include <QtCore/QObject>
#include <QtCore/QStringList>

class QuarkPlayer;

/**
 * Manages/loads QuarkPlayer plugins.
 *
 * Pattern singleton.
 *
 * @see QPluginLoader
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API PluginManager : public QObject, public Singleton<PluginManager> {
	friend class Singleton<PluginManager>;
	Q_OBJECT
public:

	/** Loads all the available plugins. */
	void loadAllPlugins(QuarkPlayer & quarkPlayer);

	/** Loads a given plugin. */
	bool loadPlugin(PluginData & pluginData);

	/**
	 * Loads/reloads a previously disabled plugin that matches the given plugin filename.
	 *
	 * If no previously disabled plugin exists then a new one will be created.
	 *
	 * <pre>
	 * PluginManager::instance().loadDisabledPlugin("name_of_my_plugin_without_file_extension");
	 * </pre>
	 *
	 * @param filename plugin to reload
	 */
	bool loadDisabledPlugin(const QString & filename);

	/**
	 * Deletes/unloads a given plugin and saves the plugins configuration.
	 *
	 * This will delete/unload all the plugins depending on the plugin asked to be deleted/unloaded.
	 */
	bool deletePlugin(PluginData & pluginData);

	/** Gets the first loaded plugin interface matching the given filename. */
	PluginInterface * pluginInterface(const QString & filename) const;

	/** Gets a plugin given a unique ID, search among all the available plugins (loaded + disabled). */
	PluginData pluginData(const QUuid & uuid) const;

	/**
	 * Gets the list of all available plugins (loaded + disabled plugins).
	 *
	 * Used by PluginConfigWidget in order to configure the plugins.
	 * @see PluginConfigWidget
	 */
	PluginDataList availablePlugins() const;

	/**
	 * @see allPluginsLoaded()
	 *
	 * Cannot be named allPluginsLoaded() that's why "already" has been added to the name.
	 */
	bool allPluginsAlreadyLoaded() const;

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
	 * So if you're slot needs a lot of time to process, it is better to use
	 * Qt::QueuedConnection
	 *
	 * @see http://doc.trolltech.com/main-snapshot/qt.html#ConnectionType-enum
	 */
	void allPluginsLoaded();

private:

	PluginManager();

	/** @see deleteAllPlugins() */
	~PluginManager();

	/**
	 * Deletes all the available plugins.
	 *
	 * Call by PluginManager destructor.
	 */
	void deleteAllPlugins();

	/** Deletes/unloads a given plugin without savinf the plugins configuration. */
	bool deletePluginWithoutSavingConfig(PluginData & pluginData);

	/** Finds the directory where dynamic plugins are located. */
	QString findPluginDir() const;

	/** QPluginLoader needs the plugin file extension unlike QLibrary who does not need it. */
	QString getRealPluginFileName(const QString & filename);

	QuarkPlayer * _quarkPlayer;

	bool _allPluginsLoaded;

	/** List of all the available plugins, i.e all the static and dynamic plugins available on the system. */
	QStringList _availablePlugins;

	/**
	 * The list of loaded/enabled plugins.
	 *
	 * This is needed when we will delete all the plugins.
	 * @see deleteAllPlugins()
	 */
	PluginDataList _loadedPlugins;

	/**
	 * The list of disabled plugins.
	 */
	PluginDataList _disabledPlugins;

	/** Directory where dynamic plugins are located. */
	QString _pluginDir;
};

#endif	//PLUGINMANAGER_H
