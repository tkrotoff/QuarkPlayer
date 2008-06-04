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

#include <QtCore/QStringList>
#include <QtCore/QDebug>

const char * Config::LANGUAGE_KEY = "language";

const char * Config::STYLE_KEY = "style";
const char * Config::ICON_THEME_KEY = "icon_theme";

const char * Config::RECENT_FILES_KEY = "recent_files";

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
	: IConfig() {

	setDefaultValues();
}

Config::~Config() {
}

void Config::setDefaultValues() {
	_keyDefaultValueMap[LANGUAGE_KEY] = QString();
	_keyDefaultValueMap[STYLE_KEY] = QString();
	_keyDefaultValueMap[ICON_THEME_KEY] = "oxygen";
	_keyDefaultValueMap[RECENT_FILES_KEY] = QStringList();
	_keyDefaultValueMap[TEST_INT_KEY] = 0;
	_keyDefaultValueMap[TEST_BOOL_KEY] = false;
}

QString Config::language() const {
	return value(LANGUAGE_KEY).toString();
}

QString Config::style() const {
	return value(STYLE_KEY).toString();
}

QStringList Config::iconThemeList() const {
	QStringList list;
	list << "Oxygen";
	list << "Silk";
	return list;
}

QString Config::iconTheme() const {
	return value(ICON_THEME_KEY).toString();
}

QStringList Config::recentFiles() const {
	return value(RECENT_FILES_KEY).toStringList();
}

int Config::testInt() const {
	return value(TEST_INT_KEY).toInt();
}

bool Config::testBool() const {
	return value(TEST_BOOL_KEY).toBool();
}
