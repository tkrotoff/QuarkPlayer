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
 * Cannot inherit from Config since instance() is a static member.
 *
 * @see Config
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
