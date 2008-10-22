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
	Config::instance().addKey(PLUGINS_KEY, QMultiMap<QString, QVariant>());
}

PluginsConfig::~PluginsConfig() {
}

PluginData::PluginList PluginsConfig::plugins() const {
	//Needs to convert from QMultiMap<QString, QVariant> to
	//QMultiMap<QString, PluginData>
	//Yes this is boring :/

	QMultiMap<QString, QVariant> original = Config::instance().value(PLUGINS_KEY).toMap();
	PluginData::PluginList convert;

	QMapIterator<QString, QVariant> it(original);
	while (it.hasNext()) {
		it.next();

		QString filename(it.key());
		QList<QVariant> values = original.values(filename);
		for (int i = 0; i < values.size(); i++) {
			QVariant value(values.at(i));
			qDebug() << __FUNCTION__ << "Key:" << filename << "value:" << value.toString();

			//FIXME Don't really know why it does not work properly
			//Let's just remove items before
			convert.remove(filename, value.toString());

			convert.insert(filename, value.toString());
		}
	}

	qDebug() << __FUNCTION__ << "Size:" << convert.count();
	return convert;
}

void PluginsConfig::setPlugins(const PluginData::PluginList & plugins) {
	//Needs to convert from QMultiMap<QString, PluginData> to
	//QMultiMap<QString, QVariant>
	//Yes this is boring :/

	qDebug() << __FUNCTION__ << "Size:" << plugins.count();

	QMultiMap<QString, QVariant> convert;
	qDebug() << __FUNCTION__ << "Size:" << convert.count();

	PluginData::PluginListIterator it(plugins);
	while (it.hasNext()) {
		it.next();

		QString filename(it.key());
		QList<PluginData> values = plugins.values(filename);
		for (int i = 0; i < values.size(); i++) {
			PluginData pluginData = values.at(i);
			qDebug() << __FUNCTION__ << "Key:" << filename << "value:" << pluginData.toString();

			//FIXME Don't really know why it does not work properly
			//Let's just remove items before
			convert.remove(filename, pluginData.toString());

			convert.insert(filename, pluginData.toString());
		}
	}

	qDebug() << __FUNCTION__ << "Size:" << convert.count();
	Config::instance().setValue(PLUGINS_KEY, convert);
}
