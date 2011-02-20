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

#include "PluginConfig.h"

#include "config/Config.h"
#include "QuarkPlayerCoreLogger.h"

const char * PluginConfig::PLUGINS_KEY = "plugins";

PluginConfig::PluginConfig() {
	Config::instance().addKey(PLUGINS_KEY, QString());
}

PluginConfig::~PluginConfig() {
}

PluginConfig & PluginConfig::instance() {
	static PluginConfig instance;
	return instance;
}

PluginDataList PluginConfig::plugins() const {
	static PluginDataList pluginList;

	if (pluginList.isEmpty()) {
		//Version working with QDataStream
		/*
		QVariant tmp = Config::instance().value(PLUGINS_KEY);
		PluginDataList pluginList = tmp.value<PluginDataList>();
		return pluginList;
		*/

		//Version working with QTextStream
		QString tmp(Config::instance().value(PLUGINS_KEY).toString());
		QTextStream stream(&tmp);
		stream >> pluginList;
	}

	return pluginList;
}

void PluginConfig::setPlugins(const PluginDataList & plugins) {
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
