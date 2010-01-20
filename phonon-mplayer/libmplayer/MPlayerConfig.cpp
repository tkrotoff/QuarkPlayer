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

#include "MPlayerConfig.h"

#include <QtCore/QDebug>

MPlayerConfig * MPlayerConfig::_instance = NULL;

MPlayerConfig::MPlayerConfig()
	:
#ifdef Q_OS_WIN
	//Forces INI file format instead of using Windows registry database
	_settings(QSettings::IniFormat, QSettings::UserScope, "phonon-mplayer", "phonon-mplayer")
#else
	_settings(QSettings::NativeFormat, QSettings::UserScope, "phonon-mplayer", "phonon-mplayer")
#endif
	{

	qDebug() << __FUNCTION__ << "Config file:" << _settings.fileName();
}

MPlayerConfig::~MPlayerConfig() {
}

MPlayerConfig & MPlayerConfig::instance() {
	if (!_instance) {
		_instance = new MPlayerConfig();
	}
	return *_instance;
}

void MPlayerConfig::deleteInstance() {
	if (_instance) {
		delete _instance;
		_instance = 0;
	}
}

QString MPlayerConfig::path() const {
#ifdef Q_OS_WIN
	//Under Windows, mplayer.exe should be inside a subdirectory named mplayer
	QString defaultMPlayerPath = "mplayer/mplayer.exe";
#else
	QString defaultMPlayerPath = "mplayer";
#endif

	return _settings.value("mplayer_path", defaultMPlayerPath).toString();
}

QString MPlayerConfig::configPath() const {
#ifdef Q_OS_WIN
	//Under Windows, config file should be inside a subdirectory named mplayer/mplayer
	QString defaultMPlayerConfigPath = "mplayer/mplayer/config";
#else
	QString defaultMPlayerConfigPath = "~/.mplayer/config";
#endif

	return _settings.value("mplayer_config_path", defaultMPlayerConfigPath).toString();
}
