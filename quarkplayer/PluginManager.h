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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <quarkplayer/quarkplayer_export.h>

#include <tkutil/Singleton.h>

#include <QtCore/QObject>
#include <QtCore/QMap>

class QuarkPlayer;
class PluginInterface;

class QPluginLoader;

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

	void loadAllPlugins(QuarkPlayer & quarkPlayer);

	void deleteAllPlugins();

	bool loadPlugin(const QString & filename);

	bool deletePlugin(const QString & filename);

	void enablePlugin(const QString & filename) const;

	void disablePlugin(const QString & filename) const;

	bool isPluginLoaded(const QString & filename) const;

	bool isPluginDisabled(const QString & filename) const;

	PluginInterface * pluginInterface(const QString & filename) const;

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

	~PluginManager();

	class PluginData {
	public:

		PluginData() {
			loader = NULL;
			interface = NULL;
		}

		QPluginLoader * loader;
		PluginInterface * interface;
	};

	typedef QMap<QString, PluginData> PluginMap;
	typedef QMapIterator<QString, PluginData> PluginMapIterator;

	/**
	 * Gets the list of plugins.
	 *
	 * Becareful: this is read/write mode (not a const reference)!
	 */
	PluginMap & pluginMap();

	PluginMap _pluginMap;

	QuarkPlayer * _quarkPlayer;
};

#endif	//PLUGINMANAGER_H
