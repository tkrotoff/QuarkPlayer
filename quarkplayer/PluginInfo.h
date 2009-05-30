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

#ifndef PLUGININFO_H
#define PLUGININFO_H

#include <QtCore/QString>

/**
 * Infos of a plugin.
 *
 * For a simple example of how to use this class, check WelcomePlugin.
 *
 * @see WelcomePlugin
 * @author Tanguy Krotoff
 */
class PluginInfo {
public:

	/**
	 * Gets the name of the plugin.
	 *
	 * @return plugin name
	 */
	virtual QString name() const = 0;

	/**
	 * List of dependencies of this plugin.
	 *
	 * @return the list of plugins that this plugin depends on
	 */
	virtual QStringList dependencies() const = 0;

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
	 * Gets the plugin webpage/URL.
	 *
	 * Example: "http://quarkplayer.googlecode.com/"
	 * Don't forget the "http://" string
	 *
	 * @return plugin webpage
	 */
	virtual QString url() const = 0;

	/**
	 * Gets the list of authors/vendor for the plugin.
	 *
	 * @return plugin author list
	 */
	virtual QString vendor() const = 0;

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
};

#endif	//PLUGININFO_H
