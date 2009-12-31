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

#include "Config.h"

#include <QtCore/QDebug>

Config * Config::_instance = NULL;

Config::Config()
	:
#ifdef Q_OS_WIN
	//INI file format, otherwise use the registry database
	_settings(QSettings::IniFormat, QSettings::UserScope, "phonon-mplayer", "phonon-mplayer")
#else
	_settings(QSettings::NativeFormat, QSettings::UserScope, "phonon-mplayer", "phonon-mplayer")
#endif
	{

	qDebug() << __FUNCTION__ << "Config file:" << _settings.fileName();
}

Config::~Config() {
}

Config & Config::instance() {
	if (!_instance) {
		_instance = new Config();
	}
	return *_instance;
}

void Config::deleteInstance() {
	if (_instance) {
		delete _instance;
		_instance = 0;
	}
}

QString Config::mplayerPath() const {
#ifdef Q_OS_WIN
	//Under Windows, mplayer.exe should be inside a subdirectory named mplayer
	QString defaultMPlayerPath = "mplayer/mplayer.exe";
#else
	QString defaultMPlayerPath = "mplayer";
#endif

	return _settings.value("mplayer_path", defaultMPlayerPath).toString();
}

QString Config::mplayerConfigPath() const {
#ifdef Q_OS_WIN
	//Under Windows, config file should be inside a subdirectory named mplayer/mplayer
	QString defaultMPlayerConfigPath = "mplayer/mplayer/config";
#else
	QString defaultMPlayerConfigPath = "~/.mplayer/config";
#endif

	return _settings.value("mplayer_config_path", defaultMPlayerConfigPath).toString();
}
