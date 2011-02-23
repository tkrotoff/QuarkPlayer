/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef MOCKPLUGINMANAGER_H
#define MOCKPLUGINMANAGER_H

#include <quarkplayer/IPluginManager.h>

class QuarkPlayer;
class PluginData;
class PluginInterface;

/**
 * Mock for IPluginManager.
 *
 * @see PluginManager
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API MockPluginManager : public IPluginManager {
	Q_OBJECT
public:

	void loadAllPlugins(QuarkPlayer & quarkPlayer);

	bool loadPlugin(PluginData & pluginData);

	bool loadDisabledPlugin(const QString & fileName);

	bool deletePlugin(PluginData & pluginData);

	PluginInterface * pluginInterface(const QString & fileName) const;

private:
};

#endif	//MOCKPLUGINMANAGER_H
