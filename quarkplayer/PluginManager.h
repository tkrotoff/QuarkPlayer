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

#include <QtCore/QObject>

class QuarkPlayer;

/**
 * Manages/loads QuarkPlayer plugins.
 *
 * Pattern singleton.
 *
 * @see QPluginLoader
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API PluginManager : public QObject {
	Q_OBJECT
public:

	~PluginManager();

	static PluginManager & instance();

	void loadPlugins(QuarkPlayer & quarkPlayer);

signals:

	void allPluginsLoaded();

private:

	PluginManager();

	template <class T>
	static QList<T> & randomize(QList<T> & list);

	/**
	 * Generates a random number between min (included) and max (included).
	 *
	 * @see http://www.geekpedia.com/tutorial39_Random-Number-Generation.html
	 */
	static int randomInt(int min, int max);

	/** Singleton. */
	static PluginManager * _pluginManager;
};

#endif	//PLUGINMANAGER_H
