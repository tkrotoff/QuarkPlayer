/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2007-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef CONFIG_H
#define CONFIG_H

#include <QtCore/QString>
#include <QtCore/QSettings>

/**
 * Read configuration file phonon-mplayer.ini.
 *
 * - Windows Vista: <pre>C:/Users/UserName/AppData/Roaming/phonon-mplayer/phonon-mplayer.ini</pre>
 * - Windows XP: <pre>C:/Documents and Settings/UserName/Application Data/phonon-mplayer/phonon-mplayer.ini</pre>
 * - UNIX: <pre>~/.config/phonon-mplayer/phonon-mplayer.conf</pre>
 *
 * Example of phonon-mplayer.ini:
 * <pre>
 * [General]
 * mplayer_path=C:/Program Files (x86)/SMPlayer/mplayer/mplayer.exe
 * mplayer_config_path=C:/Program Files (x86)/SMPlayer/mplayer/mplayer/config
 * </pre>
 * Warning! under Windows, do not use \, use / instead
 *
 * The best way to see if it works is to edit MPlayer config file and add <pre>vo="null"</pre>
 * MPlayer backend then shouldn't show any video (sound should still work).
 *
 * @author Tanguy Krotoff
 */
class MPlayerConfig {
public:

	/**
	 * Singleton.
	 *
	 * @return MPlayerConfig instance
	 */
	static MPlayerConfig & instance();

	/**
	 * Singleton.
	 *
	 * Deletes the MPlayerConfig instance.
	 */
	static void deleteInstance();

	/**
	 * Gets MPlayer binary path.
	 *
	 * Defaults:
	 * - Windows: <pre>mplayer/mplayer.exe</pre>
	 * - UNIX: <pre>mplayer</pre>
	 *
	 * @return MPlayer binary path
	 * @see MPlayerProcess
	 */
	QString path() const;

	/**
	 * Gets MPlayer config file path.
	 *
	 * Defaults:
	 * - Windows: <pre>mplayer/config</pre>
	 * - UNIX: <pre>~/.mplayer/config</pre>
	 * This is used to build MPlayer command line with argument <pre>-input</pre>
	 *
	 * @return MPlayer config file path
	 * @see MPlayerLoader
	 */
	QString configPath() const;

private:

	MPlayerConfig();

	~MPlayerConfig();

	static MPlayerConfig * _instance;

	QSettings _settings;
};

#endif	//CONFIG_H
