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

#include "PluginsConfig.h"

#include "config/Config.h"

#include <QtCore/QDebug>

const char * PluginsConfig::PLUGINS_KEY = "plugins";

PluginsConfig::PluginsConfig() {
	Config::instance().addKey(PLUGINS_KEY, QString());
}

PluginsConfig::~PluginsConfig() {
}

PluginData::PluginList PluginsConfig::plugins() const {
	//Version working with QDataStream
	/*
	QVariant tmp = Config::instance().value(PLUGINS_KEY);
	PluginData::PluginList pluginList = tmp.value<PluginData::PluginList>();
	return pluginList;
	*/

	//Version working with QTextStream
	QString tmp(Config::instance().value(PLUGINS_KEY).toString());
	QTextStream stream(&tmp);
	PluginData::PluginList pluginList;
	stream >> pluginList;
	return pluginList;
}

void PluginsConfig::setPlugins(const PluginData::PluginList & plugins) {
	//Version working with QDataStream
	/*
	QVariant tmp;
	tmp.setValue(plugins);
	Config::instance().setValue(PLUGINS_KEY, tmp);
	*/

	//Version working with QTextStream
	QString tmp;
	QTextStream stream(&tmp);
	stream << plugins;
	Config::instance().setValue(PLUGINS_KEY, tmp);
}
