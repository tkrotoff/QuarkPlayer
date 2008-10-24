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

#ifndef PLUGINSCONFIG_H
#define PLUGINSCONFIG_H

#include <quarkplayer/PluginData.h>

#include <tkutil/Singleton.h>

/**
 * Load and save QuarkPlayer plugins configuration.
 *
 * Cannot inherit directly from Config since Config::instance() is a static member.
 *
 * Problem saving (serialization) the list of plugins:
 * QSettings works with QVariant. QVariant works with QDataStream
 * QDataStream does not give us human readable datas.
 * QVariant does not really work with QTextStream.
 * Still we use QTextStream since it gives us a human readable format.
 *
 * Maybe plugin list should be saved in its own XML/ini file?
 *
 * @see Config
 * @see PluginData
 * @author Tanguy Krotoff
 */
class PluginsConfig : public Singleton<PluginsConfig> {
	friend class Singleton<PluginsConfig>;
public:

	/**
	 * Plugins list.
	 *
	 * This is used by PluginManager at launch time.
	 *
	 * @see PluginManager
	 */
	static const char * PLUGINS_KEY;
	PluginData::PluginList plugins() const;
	void setPlugins(const PluginData::PluginList & plugins);

private:

	PluginsConfig();

	~PluginsConfig();
};

#endif	//PLUGINSCONFIG_H
