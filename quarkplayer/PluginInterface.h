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

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <quarkplayer/quarkplayer_export.h>

#include <QtCore/QString>
#include <QtCore/QUuid>

class QuarkPlayer;

/**
 * Interface for a QuarkPlayer plugin.
 *
 * You should inherit from this interface in order to create a plugin for QuarkPlayer.
 *
 * For an simple example of how to use this class, check WelcomePlugin.
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
	 * Gets the name of the plugin.
	 *
	 * FIXME All these functions might be moved to a new
	 * class PluginInfoInterface if the need comes.
	 *
	 * @return plugin name
	 */
	virtual QString name() const = 0;

	/**
	 * Gets a short description of what does the plugin.
	 *
	 * @return plugin short description
	 */
	virtual QString description() const = 0;

	/**
	 * Gets the version number of the plugin (example: "1.1.4").
	 *
	 * @return plugin version number
	 */
	virtual QString version() const = 0;

	/**
	 * Gets the plugin webpage.
	 *
	 * Example: "http://quarkplayer.googlecode.com/"
	 * Don't forget the "http://" string
	 *
	 * FIXME should return a QUrl?
	 *
	 * @return plugin webpage
	 */
	virtual QString webpage() const = 0;

	/**
	 * Gets the email address associated with the plugin.
	 *
	 * Can be the email address of the author or a mailing-list email address.
	 * Example: quarkplayer@googlegroups.com
	 *
	 * @return plugin email address
	 */
	virtual QString email() const = 0;

	/**
	 * Gets the list of authors for the plugin.
	 *
	 * FIXME should return a QStringList?
	 *
	 * @return plugin author list
	 */
	virtual QString authors() const = 0;

	/**
	 * Gets the license of the plugin.
	 *
	 * Example: "GNU GPLv3+"
	 *
	 * @return plugin license
	 */
	virtual QString license() const = 0;

	/**
	 * Gets the copyright associated with the plugin.
	 *
	 * In general it will be:
	 * "Copyright (C) AuthorFirstname AuthoLastname"
	 *
	 * @return plugin copyright
	 */
	virtual QString copyright() const = 0;

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
