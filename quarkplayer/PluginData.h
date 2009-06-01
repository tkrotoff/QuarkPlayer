/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <quarkplayer/quarkplayer_export.h>

#include <QtCore/QUuid>
#include <QtCore/QMultiHash>
#include <QtCore/QTextStream>

class PluginInterface;
class PluginFactory;

/**
 * Informations about a plugin.
 *
 * One plugin can be loaded several times.
 * This is the case for plugins 'filebrowser' and 'playlist'
 * Example:
 * <pre>
 * filebrowser => UUID={fb4ec4ac-5483-4bb2-96ad-807d1ec04f24} | enabled=1
 * filebrowser => UUID={6d44588d-a374-41f8-b8f7-1d5c3f072384} | enabled=1
 * filebrowser => UUID={8cd6c48b-3b8e-405c-853e-e25e6ca693c5} | enabled=0
 * </pre>
 * We have 2 different filebrowser plugins (with different UUID i.e unique ID) that are enabled (i.e displayed)
 * and a third filebrowser plugin that is disabled.
 * That's why each plugin instance matches a unique ID (UUID).
 *
 * <pre>PluginInterface *</pre> and <pre>QPluginLoader *</pre> are here for loading/deleting a plugin
 * at runtime without having the user to restart the application after modifying the plugins configuration.
 *
 * @see PluginManager
 * @see PluginConfig
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API PluginData {
public:

	PluginData(const QString & filename, const QUuid & uuid, bool enabled);

	PluginData(const PluginData & pluginData);

	PluginData();

	~PluginData();

	PluginData & operator=(const PluginData & pluginData);

	/** Comparison based on UUID. */
	int operator==(const PluginData & pluginData);

	QString fileName() const;

	QUuid uuid() const;

	/** Is the plugin enabled or disabled? */
	bool isEnabled() const;
	void setEnabled(bool enabled);

	PluginFactory * factory() const;
	void setFactory(PluginFactory * factory);

	PluginInterface * interface() const;
	void setInterface(PluginInterface * interface);

	/** Deletes the PluginInterface. */
	void deleteInterface();

private:

	void copy(const PluginData & pluginData);

	QString _filename;

	QUuid _uuid;

	bool _enabled;

	PluginInterface * _interface;

	PluginFactory * _factory;
};


/**
 * List of PluginData.
 */
class PluginDataList : public QList<PluginData> {
public:

	/** Gets the sublist of plugins matching the given filename. */
	PluginDataList values(const QString & filename) const;
};


QDataStream & operator<<(QDataStream & stream, const PluginDataList & plugins);

QDataStream & operator>>(QDataStream & stream, PluginDataList & plugins);

QTextStream & operator<<(QTextStream & stream, const PluginDataList & plugins);

QTextStream & operator>>(QTextStream & stream, PluginDataList & plugins);

#endif	//PLUGINDATA_H
