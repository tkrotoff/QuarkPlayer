/*
 * VLC backend for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "vlc_symbols.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QtDebug>
#include <QtCore/QLibrary>
#include <QtCore/QStringList>
#include <QtCore/QDir>

static QLibrary * _libvlc_control = NULL;

QString getVLCPath() {
	static const char * libvlc_control_name = "libvlc-control";
	static const char * libvlc_control_functionToTest = "libvlc_exception_init";

	static QString libvlc_path;

	if (!libvlc_path.isEmpty()) {
		return libvlc_path;
	}

	//Tries to autodetect the VLC path with a default list of path

	QStringList pathList;
	pathList << QCoreApplication::libraryPaths();
	pathList << "/usr/local/lib";

	_libvlc_control = new QLibrary();

	foreach (libvlc_path, pathList) {
		_libvlc_control->setFileName(libvlc_path + QDir::separator() + libvlc_control_name);

		if (_libvlc_control->load() && _libvlc_control->resolve(libvlc_control_functionToTest)) {
			qDebug() << "VLC path found:" << libvlc_path;
			return libvlc_path;
		}
		qDebug() << "Warning:" << _libvlc_control->errorString();
	}

	unloadLibVLC();
	qFatal("Cannot find '%s' on your system", libvlc_control_name);
	return libvlc_path;
}

QString getVLCPluginsPath() {
	QString vlcPath = getVLCPath();

#ifdef Q_OS_WIN
	QString vlcPluginsPath(vlcPath + "/plugins");
#endif	//Q_OS_WIN

#ifdef Q_OS_LINUX
	QString vlcPluginsPath(vlcPath + "/vlc");
#endif	//Q_OS_LINUX

	qDebug() << "VLC plugins path:" << vlcPluginsPath;

	return vlcPluginsPath;
}

QString getLibVLCFilename() {
	QString vlcDll(getVLCPath() + "/libvlc-control");
	return vlcDll;
}

void * resolve(const char * name) {
	if (!_libvlc_control) {
		qFatal("_libvlc_control cannot be NULL");
	}

	if (!_libvlc_control->isLoaded()) {
		qFatal("Library '%s' not loaded", _libvlc_control->fileName());
		return NULL;
	}

	void * func = _libvlc_control->resolve(name);
	if (!func) {
		qFatal("Cannot resolve '%s' in library '%s'", name, _libvlc_control->fileName());
	}

	return func;
}

void unloadLibVLC() {
	_libvlc_control->unload();
	delete _libvlc_control;
	_libvlc_control = NULL;
}
