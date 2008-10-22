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
#include <QtCore/QUuid>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

const char * Config::BACKEND_KEY = "backend";

const char * Config::LANGUAGE_KEY = "language";

const char * Config::STYLE_KEY = "style";
const char * Config::ICON_THEME_KEY = "icon_theme";

const char * Config::RECENT_FILES_KEY = "recent_files";

const char * Config::LAST_DIRECTORY_USED_KEY = "last_directory_used";

const char * Config::LAST_VOLUME_USED_KEY = "last_volume_used";
const char * Config::VOLUME_MUTED_KEY = "volume_muted";

const char * Config::MUSIC_DIR_KEY = "music_dir";

const char * Config::PLUGINS_DIR_KEY = "plugins_dir";

const char * Config::MAINWINDOW_GEOMETRY_KEY = "mainwindow_geometry";

Config::Config()
	: TkConfig() {

	addKey(BACKEND_KEY, "mplayer");
	addKey(LANGUAGE_KEY, QString());
	addKey(STYLE_KEY, "QuarkPlayerStyle");
	addKey(ICON_THEME_KEY, "silk");
	addKey(RECENT_FILES_KEY, QStringList());
	addKey(LAST_DIRECTORY_USED_KEY, QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
	addKey(LAST_VOLUME_USED_KEY, 1.0f);
	addKey(VOLUME_MUTED_KEY, false);
	addKey(MUSIC_DIR_KEY, QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
	addKey(PLUGINS_DIR_KEY, QString(QCoreApplication::applicationDirPath() + "/plugins"));
	addKey(MAINWINDOW_GEOMETRY_KEY, QByteArray());
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
		<< "Tango"
		<< "Crystal"
		<< "GartoonRedux"
		<< "Null";
	return list;
}

QString Config::iconTheme() const {
	return value(ICON_THEME_KEY).toString();
}

QStringList Config::recentFiles() const {
	QSet<QString> tmp = value(RECENT_FILES_KEY).toStringList().toSet();
	return tmp.toList();
}

QString Config::lastDirectoryUsed() const {
	return value(LAST_DIRECTORY_USED_KEY).toString();
}

qreal Config::lastVolumeUsed() const {
	//Between 0.0 and 1.0
	return value(LAST_VOLUME_USED_KEY).toDouble();
}

bool Config::volumeMuted() const {
	return value(VOLUME_MUTED_KEY).toBool();
}

void Config::addMusicDir(const QString & musicDir, const QUuid & uuid) {
	if (uuid.isNull()) {
		qCritical() << __FUNCTION__ << "Error: empty UUID";
	}

	QString key(MUSIC_DIR_KEY + uuid.toString());
	if (!contains(key)) {
		//qCritical() << __FUNCTION__ << "Error: unknown UUID:" << uuid;
		addKey(key, defaultValue(MUSIC_DIR_KEY).toString());
	}
	setValue(key, musicDir);
}

QString Config::musicDir(const QUuid & uuid) /*const*/ {
	QString key(MUSIC_DIR_KEY + uuid.toString());
	if (!contains(key)) {
		//qCritical() << __FUNCTION__ << "Error: unknown UUID:" << uuid;
		addKey(key, defaultValue(MUSIC_DIR_KEY).toString());
	}
	return value(key).toString();
}

QString Config::pluginsDir() const {
	return value(PLUGINS_DIR_KEY).toString();
}

QByteArray Config::mainWindowGeometry() const {
	return value(MAINWINDOW_GEOMETRY_KEY).toByteArray();
}
