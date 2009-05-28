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

#ifndef PLUGINSPEC_H
#define PLUGINSPEC_H

/**
 * Parses the XML from a .pluginspec file.
 *
 * .pluginspec file format inspired by Qt Creator,
 * this is a simplified version.
 *
 * Example of a .pluginspec file:
 * <pre>
 * <plugin name="example" version="0.1">
 * 	<vendor>Tanguy Krotoff</vendor>
 * 	<copyright>(C) 2008 Tanguy Krotoff</copyright>
 * 	<license>GNU GPLv3+</license>
 * 	<description>Example plugin</description>
 * 	<url>http://quarkplayer.googlecode.com/</url>
 * </plugin>
 * </pre>
 *
 *
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API PluginSpec {
public:

	static const char * FILE_EXTENSION;

	PluginSpec();

	~PluginSpec();

	bool read(const QString & fileName);

	/**
	 * Gets the name of the plugin.
	 *
	 * @return plugin name
	 */
	QString name() const;

	/**
	 * Gets the version number of the plugin (example: "1.1.4").
	 *
	 * @return plugin version number
	 */
	QString version() const;

	/**
	 * Gets the vendor (or authors) for the plugin.
	 *
	 * @return plugin vendor
	 */
	QString vendor() const;

	/**
	 * Gets the copyright associated with the plugin.
	 *
	 * In general it will be:
	 * "(C) 2008 AuthorFirstname AuthorLastname"
	 *
	 * @return plugin copyright
	 */
	QString copyright() const;

	/**
	 * Gets the license of the plugin.
	 *
	 * Example: "GNU GPLv3+"
	 *
	 * @return plugin license
	 */
	QString license() const;

	/**
	 * Gets a short description of what does the plugin.
	 *
	 * @return plugin short description
	 */
	QString description() const;

	/**
	 * Gets the plugin url/webpage.
	 *
	 * Example: "http://quarkplayer.googlecode.com/"
	 * Don't forget the "http://" string
	 *
	 * FIXME should return a QUrl?
	 *
	 * @return plugin url/webpage
	 */
	QString url() const;

	bool hasError() const;
	QString errorString() const;

private:

	QString _name;
	QString _version;
	QString _vendor;
	QString _copyright;
	QString _license;
	QString _description;
	QString _url;

	bool _hasError;
	QString _errorString;
};

#endif	//PLUGINSPEC_H
