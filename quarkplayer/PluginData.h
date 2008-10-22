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

#ifndef PLUGINDATA_H
#define PLUGINDATA_H

#include <QtCore/QUuid>
#include <QtCore/QMultiMap>

class PluginInterface;

class QPluginLoader;

/**
 * Informations about a plugin.
 *
 * @see PluginManager
 * @see PluginsConfig
 * @author Tanguy Krotoff
 */
class PluginData {
public:

	/** QMultiMap<QString, PluginData> with QString == plugin filename. */
	typedef QMultiMap<QString, PluginData> PluginList;

	typedef QMapIterator<QString, PluginData> PluginListIterator;


	PluginData(const QUuid & uuid, bool enabled);

	PluginData(const PluginData & pluginData);

	PluginData(const QString & data);

	PluginData();

	~PluginData();

	PluginData & operator=(const PluginData & right);

	/** Comparison based on UUID. */
	int operator==(const PluginData & right);

	QString toString() const;

	QUuid uuid() const;

	bool isEnabled() const;
	void setEnabled(bool enabled);

	QPluginLoader * loader() const;
	void setLoader(QPluginLoader * loader);
	void deleteLoader();

	PluginInterface * interface() const;
	void setInterface(PluginInterface * interface);

	//FIXME this does not work, it crashes using MSVC80
	void deleteInterface();

private:

	void copy(const PluginData & pluginData);

	QUuid _uuid;

	bool _enabled;

	QPluginLoader * _loader;

	PluginInterface * _interface;
};

#endif	//PLUGINDATA_H
