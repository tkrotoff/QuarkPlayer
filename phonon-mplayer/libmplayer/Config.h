/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2007-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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
 * - Windows XP: <pre>UserName/phonon-mplayer/phonon-mplayer.ini</pre>
 * - UNIX: <pre>~/.phonon-mplayer/phonon-mplayer.ini</pre>
 *
 * @author Tanguy Krotoff
 */
class Config {
public:

	static Config & instance();

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
	QString mplayerPath() const;

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
	QString mplayerConfigPath() const;

private:

	Config();

	~Config();

	static Config * _instance;

	QSettings _settings;
};

#endif	//CONFIG_H
