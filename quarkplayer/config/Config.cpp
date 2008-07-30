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

#include "Config.h"

#include <QtGui/QDesktopServices>

#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

const char * Config::BACKEND_KEY = "backend";

const char * Config::LANGUAGE_KEY = "language";

const char * Config::STYLE_KEY = "style";
const char * Config::ICON_THEME_KEY = "icon_theme";

const char * Config::RECENT_FILES_KEY = "recent_files";

const char * Config::LAST_DIRECTORY_USED_KEY = "last_directory_used";

const char * Config::LAST_VOLUME_USED_KEY = "last_volume_used";

const char * Config::MUSIC_DIR_KEY = "music_dir";

const char * Config::PLUGINS_DIR_KEY = "plugins_dir";
const char * Config::PLUGINS_DISABLED_KEY = "plugins_disabled";


const char * Config::TEST_INT_KEY = "test_int";
const char * Config::TEST_BOOL_KEY = "test_bool";


Config * Config::_instance = NULL;

Config & Config::instance() {
	//Lazy initialization
	if (!_instance) {
		_instance = new Config();
	}

	return *_instance;
}

Config::Config()
	: TkConfig() {

	addKey(BACKEND_KEY, "mplayer");
	addKey(LANGUAGE_KEY, QString());
	addKey(STYLE_KEY, "QuarkPlayerStyle");
	addKey(ICON_THEME_KEY, "silk");
	addKey(RECENT_FILES_KEY, QStringList());
	addKey(LAST_DIRECTORY_USED_KEY, QString());
	addKey(LAST_VOLUME_USED_KEY, 1.0f);
	addKey(MUSIC_DIR_KEY, QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
	addKey(PLUGINS_DIR_KEY, QString(QCoreApplication::applicationDirPath() + "/plugins"));
	addKey(PLUGINS_DISABLED_KEY, QStringList());

	addKey(TEST_INT_KEY, 0);
	addKey(TEST_BOOL_KEY, false);
}

Config::~Config() {
}

QStringList Config::backendList() const {
	QStringList list;
	list << "MPlayer"
		<< "VLC"
		<< "DS9";
	return list;
}

QString Config::backend() const {
	return value(BACKEND_KEY).toString();
}

QString Config::language() const {
	return value(LANGUAGE_KEY).toString();
}

QString Config::style() const {
	return value(STYLE_KEY).toString();
}

QStringList Config::iconThemeList() const {
	QStringList list;
	list << "Oxygen"
		<< "Silk"
		<< "Tango";
	return list;
}

QString Config::iconTheme() const {
	return value(ICON_THEME_KEY).toString();
}

QStringList Config::recentFiles() const {
	return value(RECENT_FILES_KEY).toStringList();
}

QString Config::lastDirectoryUsed() const {
	return value(LAST_DIRECTORY_USED_KEY).toString();
}

qreal Config::lastVolumeUsed() const {
	//Between 0.0 and 1.0
	return value(LAST_VOLUME_USED_KEY).toDouble();
}

QString Config::musicDir() const {
	return value(MUSIC_DIR_KEY).toString();
}

QString Config::pluginsDir() const {
	return value(PLUGINS_DIR_KEY).toString();
}

QStringList Config::pluginsDisabled() const {
	return value(PLUGINS_DISABLED_KEY).toStringList();
}






int Config::testInt() const {
	return value(TEST_INT_KEY).toInt();
}

bool Config::testBool() const {
	return value(TEST_BOOL_KEY).toBool();
}
