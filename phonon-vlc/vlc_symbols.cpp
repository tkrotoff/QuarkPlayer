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
#include <QtCore/QSettings>
#include <QtCore/QtGlobal>

#ifdef Q_OS_WIN
	#include <windows.h>

	char currentDirectory[MAX_PATH];
#endif	//Q_OS_WIN

static QLibrary * _libvlc_control = NULL;

/**
 * Only under Windows.
 *
 * Use Win32 API function: GetCurrentDirectory()
 *
 * @see setCurrentDirectory()
 * @see changeBackToCurrentDirectory()
 */
void saveCurrentDirectory() {
#ifdef Q_OS_WIN
	DWORD dwRet = GetCurrentDirectoryA(MAX_PATH, currentDirectory);
	if (dwRet == 0 || dwRet > MAX_PATH) {
		qFatal("GetCurrentDirectory() failed (%d)", GetLastError());
	}
#endif	//Q_OS_WIN
}

/**
 * Only under Windows.
 *
 * Use Win32 API function: SetCurrentDirectory()
 *
 * @see saveCurrentDirectory()
 * @see changeBackToCurrentDirectory()
 */
void setCurrentDirectory(const char * dir) {
#ifdef Q_OS_WIN
	//Change current directory in order to load all the *.dll (libvlc.dll + all the plugins)
	if (!SetCurrentDirectoryA(dir)) {
		qFatal("SetCurrentDirectory() failed (%d)\n", GetLastError());
	}
#endif	//Q_OS_WIN
}

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

#ifdef Q_OS_LINUX
	pathList << "/usr/local/lib";
#endif	//Q_OS_LINUX

#ifdef Q_OS_WIN
	static const char * libvlc_version = "0.9";

	saveCurrentDirectory();

	//QSettings allows us to read the Windows registry
	//Check if there is a standard VLC installation under Windows
	//If there is a VLC Windows installation, check we get the good version i.e 0.9
	QSettings settings(QSettings::SystemScope, "VideoLAN", "VLC");
	if (settings.value("Version").toString().contains(libvlc_version)) {
		QString vlcInstallDir = settings.value("InstallDir").toString();
		pathList << vlcInstallDir;

		setCurrentDirectory(vlcInstallDir.toAscii().constData());
	}
#endif	//Q_OS_WIN

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

void changeBackToCurrentDirectory() {
#ifdef Q_OS_WIN
	//Change back current directory
	if (!SetCurrentDirectoryA(currentDirectory)) {
		qFatal("SetCurrentDirectory() failed (%d)\n", GetLastError());
	}
#endif	//Q_OS_WIN
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

void * resolve(const char * name) {
	if (!_libvlc_control) {
		qFatal("_libvlc_control cannot be NULL");
	}

	if (!_libvlc_control->isLoaded()) {
		qFatal("Library '%s' not loaded", _libvlc_control->fileName().toAscii().constData());
		return NULL;
	}

	void * func = _libvlc_control->resolve(name);
	if (!func) {
		qFatal("Cannot resolve '%s' in library '%s'", name, _libvlc_control->fileName().toAscii().constData());
	}

	return func;
}

void unloadLibVLC() {
	_libvlc_control->unload();
	delete _libvlc_control;
	_libvlc_control = NULL;
}
