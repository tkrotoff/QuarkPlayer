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

#include "Config.h"

#include <QtGui/QDesktopServices>

#include <QtCore/QStringList>
#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

const char * Config::BACKEND_KEY = "backend";

const char * Config::LANGUAGE_KEY = "language";

const char * Config::STYLE_KEY = "style";
const char * Config::ICON_THEME_KEY = "icon_theme";

const char * Config::RECENT_FILES_KEY = "recent_files";

const char * Config::LAST_DIR_OPENED_KEY = "last_dir_opened";
const char * Config::DVD_DIR_KEY = "dvd_dir";
const char * Config::CDROM_DIR_KEY = "cdrom_dir";
const char * Config::MUSIC_DIR_KEY = "music_dir";
const char * Config::PLUGIN_DIR_KEY = "plugin_dir";
const char * Config::RESOURCE_DIR_KEY = "resource_dir";

const char * Config::LAST_VOLUME_USED_KEY = "last_volume_used";
const char * Config::VOLUME_MUTED_KEY = "volume_muted";

const char * Config::MAINWINDOW_GEOMETRY_KEY = "mainwindow_geometry";

const char * Config::ALLOW_MULTIPLE_INSTANCES_KEY = "allow_multiple_instances";

Config::Config()
	: TkConfig() {

	addKey(BACKEND_KEY, "mplayer");
	addKey(LANGUAGE_KEY, QString());
	addKey(STYLE_KEY, "QuarkPlayerStyle");
	addKey(ICON_THEME_KEY, "silk");
	addKey(RECENT_FILES_KEY, QStringList());
	addKey(LAST_DIR_OPENED_KEY, QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));

	//Default DVD and CDROM device
	QString dvdDir;
	QString cdromDir;
#ifdef Q_OS_WIN
	dvdDir = "D:/";
	cdromDir = "D:/";
#elif defined(Q_OS_MAC)
	//FIXME don't know yet
	dvdDir = '/';
	cdromDir = '/';
#elif defined(Q_OS_UNIX)
	dvdDir = "/dev/dvd";
	cdromDir = "/dev/cdrom";
#endif
	addKey(DVD_DIR_KEY, dvdDir);
	addKey(CDROM_DIR_KEY, cdromDir);

	addKey(MUSIC_DIR_KEY, QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

	//Order matters!!! See PluginManager.cpp
	//All possible plugin directories
	QStringList pluginDirList;
	pluginDirList << QCoreApplication::applicationDirPath() + "/plugins";
#ifdef Q_OS_UNIX
	pluginDirList << "/usr/lib/quarkplayer/plugins";
#endif	//Q_OS_UNIX
	addKey(PLUGIN_DIR_KEY, pluginDirList);
	///

	addKey(RESOURCE_DIR_KEY, QString(QCoreApplication::applicationDirPath()));

	addKey(LAST_VOLUME_USED_KEY, 1.0f);
	addKey(VOLUME_MUTED_KEY, false);
	addKey(MAINWINDOW_GEOMETRY_KEY, QByteArray());
}

Config::~Config() {
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

QString Config::lastDirOpened() const {
	return value(LAST_DIR_OPENED_KEY).toString();
}

QString Config::dvdDir() const {
	return value(DVD_DIR_KEY).toString();
}

QString Config::cdromDir() const {
	return value(CDROM_DIR_KEY).toString();
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
