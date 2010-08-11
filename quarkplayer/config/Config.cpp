/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <quarkplayer/QuarkPlayerCoreLogger.h>

#include <QtGui/QDesktopServices>

#include <QtCore/QStringList>
#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

const char * Config::LANGUAGE_KEY = "language";

const char * Config::STYLE_KEY = "style";
const char * Config::ICON_THEME_KEY = "icon_theme";

const char * Config::RECENT_FILES_KEY = "recent_files";

const char * Config::LAST_DIR_OPENED_KEY = "last_dir_opened";
const char * Config::DVD_PATH_KEY = "dvd_path";
const char * Config::CDROM_PATH_KEY = "cdrom_path";
const char * Config::MUSIC_DIR_KEY = "music_dir";
const char * Config::PLUGIN_DIR_KEY = "plugin_dir";
const char * Config::RESOURCE_DIR_KEY = "resource_dir";

const char * Config::LAST_VOLUME_USED_KEY = "last_volume_used";
const char * Config::VOLUME_MUTED_KEY = "volume_muted";

const char * Config::MAINWINDOW_GEOMETRY_KEY = "mainwindow_geometry";

const char * Config::ALLOW_MULTIPLE_INSTANCES_KEY = "allow_multiple_instances";

const char * Config::FINDFILES_BACKEND_KEY = "findfiles_backend";

Config::Config()
	: TkConfig() {

	addKey(LANGUAGE_KEY, QString());
	addKey(STYLE_KEY, QString());
	addKey(ICON_THEME_KEY, QString());
	addKey(RECENT_FILES_KEY, QStringList());
	addKey(LAST_DIR_OPENED_KEY, QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));

	//Default DVD and CDROM device
	QString dvdPath;
	QString cdromPath;
#ifdef Q_WS_WIN
	dvdPath = "D:/";
	cdromPath = "D:/";
#elif defined(Q_WS_MAC)
	//FIXME don't know yet
	dvdPath = '/';
	cdromPath = '/';
#elif defined(Q_WS_X11)
	dvdPath = "/dev/sr0";
	cdromPath = "/dev/sr0";
#endif
	addKey(DVD_PATH_KEY, dvdPath);
	addKey(CDROM_PATH_KEY, cdromPath);

	addKey(MUSIC_DIR_KEY, QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

	//Order matters!!! See PluginManager.cpp
	//All possible plugin directories
	QStringList pluginDirList;
	pluginDirList << QCoreApplication::applicationDirPath() + "/plugins";
#ifdef Q_WS_X11
	pluginDirList << "/usr/lib/quarkplayer/plugins";
#endif	//Q_WS_X11
	addKey(PLUGIN_DIR_KEY, pluginDirList);
	///

	addKey(RESOURCE_DIR_KEY, QString(QCoreApplication::applicationDirPath()));

	addKey(LAST_VOLUME_USED_KEY, 1.0f);
	addKey(VOLUME_MUTED_KEY, false);
	addKey(MAINWINDOW_GEOMETRY_KEY, QByteArray());

	addKey(ALLOW_MULTIPLE_INSTANCES_KEY, false);

	//0 = native, 1 = Qt
	addKey(FINDFILES_BACKEND_KEY, 0);
}

Config::~Config() {
}

Config & Config::instance() {
	static Config instance;
	return instance;
}

void Config::deleteConfig() {
	//Needs to remove all the files before to be able to remove the directory
	//This is the way QDir::rmdir() works :/
	//This code supposes that there is no directory inside the config directory
	//FIXME switch to recursive code?

	QDir configDir = QFileInfo(fileName()).absoluteDir();
	QStringList configFiles = configDir.entryList();
	foreach(QString file, configFiles) {
		//Remove all the files
		configDir.remove(file);
	}

	//The directory should be empty, let's remove it
	QString tmp(configDir.dirName());
	configDir.cdUp();
	configDir.rmdir(tmp);
}

QString Config::language() const {
	return value(LANGUAGE_KEY).toString();
}

QString Config::style() const {
	return value(STYLE_KEY).toString();
}

QString Config::iconTheme() const {
	return value(ICON_THEME_KEY).toString();
}

QStringList Config::recentFiles() const {
	QSet<QString> tmp = value(RECENT_FILES_KEY).toStringList().toSet();
	return tmp.toList();
}

QString Config::lastDirOpened() const {
	return value(LAST_DIR_OPENED_KEY).toString();
}

QString Config::dvdPath() const {
	return value(DVD_PATH_KEY).toString();
}

QString Config::cdromPath() const {
	return value(CDROM_PATH_KEY).toString();
}

void Config::addMusicDir(const QString & musicDir, const QUuid & uuid) {
	if (uuid.isNull()) {
		QuarkPlayerCoreCritical() << "Error: empty UUID";
	}

	QString key(MUSIC_DIR_KEY + uuid.toString());
	if (!contains(key)) {
		//QuarkPlayerCoreCritical() << "Error: unknown UUID:" << uuid;
		addKey(key, defaultValue(MUSIC_DIR_KEY).toString());
	}
	setValue(key, musicDir);
}

QString Config::musicDir(const QUuid & uuid) /*const*/ {
	QString key(MUSIC_DIR_KEY + uuid.toString());
	if (!contains(key)) {
		//QuarkPlayerCoreCritical() << "Error: unknown UUID:" << uuid;
		addKey(key, defaultValue(MUSIC_DIR_KEY).toString());
	}
	return value(key).toString();
}

QStringList Config::pluginDirList() const {
	return value(PLUGIN_DIR_KEY).toStringList();
}

QString Config::resourceDir() const {
	return value(RESOURCE_DIR_KEY).toString();
}

qreal Config::lastVolumeUsed() const {
	//Between 0.0 and 1.0
	return value(LAST_VOLUME_USED_KEY).toDouble();
}

bool Config::volumeMuted() const {
	return value(VOLUME_MUTED_KEY).toBool();
}

QByteArray Config::mainWindowGeometry() const {
	return value(MAINWINDOW_GEOMETRY_KEY).toByteArray();
}

bool Config::allowMultipleInstances() const {
	return value(ALLOW_MULTIPLE_INSTANCES_KEY).toBool();
}

int Config::findFilesBackend() const {
	return value(FINDFILES_BACKEND_KEY).toInt();
}
