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

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <quarkplayer/quarkplayer_export.h>

#include <QtCore/QString>
#include <QtCore/QUuid>

class QuarkPlayer;

class QAction;

/**
 * Interface for a QuarkPlayer plugin.
 *
 * You should inherit from this interface in order to create a plugin for QuarkPlayer.
 *
 * For a simple example of how to use this class, check WelcomePlugin.
 *
 * @see PluginFactory
 * @see QuarkPlayer
 * @see WelcomePlugin
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API PluginInterface {
public:

	/**
	 * Creates a new plugin.
	 *
	 * PluginFactory is the one that should call this constructor.
	 *
	 * @param quarkPlayer QuarkPlayer main API
	 * @param uuid unique identifier for the plugin
	 * @see quarkPlayer()
	 * @see uuid()
	 * @see PluginFactory
	 */
	PluginInterface(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	/**
	 * Plugin destruction.
	 *
	 * You should "unplug" from QuarkPlayer API here and
	 * delete graphically your plugin.
	 * Example: if your plugin have added items to QuarkPlayer menu bar,
	 * then you should delete these items from the menu bar
	 */
	virtual ~PluginInterface();

	/**
	 * Gets the unique ID of this plugin.
	 *
	 * @return this plugin unique ID
	 */
	QUuid uuid() const;

	/**
	 * Gets a QAction given a name and the uuid of the plugin.
	 *
	 * Helper/factorization function.
	 *
	 * @see ActionCollection
	 */
	QAction * uuidAction(const QString & name);

	/**
	 * Adds a QAction given a name and the uuid of the plugin.
	 *
	 * Helper/factorization function.
	 *
	 * @see ActionCollection
	 */
	void addUuidAction(const QString & name, QAction * action);

protected:

	/**
	 * Gets QuarkPlayer main API.
	 *
	 * This is a useful method that let's you easily deal
	 * with QuarkPlayer API.
	 *
	 * API means Application programming interface
	 *
	 * @see http://en.wikipedia.org/wiki/API
	 * @return QuarkPlayer API
	 */
	QuarkPlayer & quarkPlayer() const;

private:

	/**
	 * QuarkPlayer main API.
	 *
	 * @see quarkPlayer()
	 */
	QuarkPlayer & _quarkPlayer;

	/**
	 * Plugin unique ID.
	 *
	 * @see uuid()
	 */
	QUuid _uuid;
};

#endif	//PLUGININTERFACE_H
