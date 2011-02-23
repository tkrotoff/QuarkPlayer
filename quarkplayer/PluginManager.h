/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <quarkplayer/IPluginManager.h>

#include <TkUtil/Singleton.h>

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
class QUARKPLAYER_API PluginManager : public IPluginManager, public Singleton {
	Q_OBJECT
public:

	/** Singleton. */
	static PluginManager & instance();

	void loadAllPlugins(QuarkPlayer & quarkPlayer);

	bool loadPlugin(PluginData & pluginData);

	bool loadDisabledPlugin(const QString & fileName);

	bool deletePlugin(PluginData & pluginData);

	PluginInterface * pluginInterface(const QString & fileName) const;

	PluginData pluginData(const QUuid & uuid) const;

	PluginDataList availablePlugins() const;

	bool allPluginsAlreadyLoaded() const;

private:

	PluginManager();

	/** @see deleteAllPlugins() */
	~PluginManager();

	/** Saves the plugins configuration. */
	void savePluginConfig();

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
	QString getRealPluginFileName(const QString & fileName);

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
