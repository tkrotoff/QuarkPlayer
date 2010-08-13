/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "ScreenSaver.h"

#include "TkUtilLogger.h"

#ifdef Q_WS_WIN
	#include <windows.h>

	int _lowPowerTimeout = 0;
	int _powerOffTimeout = 0;
	int _screenSaverTimeout = 0;
	bool _stateSaved = false;
#endif	//Q_WS_WIN

#ifdef Q_WS_X11
	#include <QtGui/QApplication>
	#include <QtGui/QWidget>
	#include <QtCore/QProcess>

	QProcess * _xdgScreenSaverProcess = NULL;
	WId _XWindowID = 0;
#endif	//Q_WS_X11

void ScreenSaver::disable() {
	TkUtilDebug();

#ifdef Q_WS_WIN
	if (!_stateSaved) {
		SystemParametersInfo(SPI_GETLOWPOWERTIMEOUT, 0, &_lowPowerTimeout, 0);
		SystemParametersInfo(SPI_GETPOWEROFFTIMEOUT, 0, &_powerOffTimeout, 0);
		SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &_screenSaverTimeout, 0);
		_stateSaved = true;
	}

	TkUtilDebug() << "lowPowerTimeout:" << _lowPowerTimeout;
	TkUtilDebug() << "powerOffTimeout:" << _powerOffTimeout;
	TkUtilDebug() << "screenSaverTimeout:" << _screenSaverTimeout;

	SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT, 0, NULL, 0);
	SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT, 0, NULL, 0);
	SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 0, NULL, 0);
#endif	//Q_WS_WIN

#ifdef Q_WS_X11
	if (!_xdgScreenSaverProcess) {
		//Lazy initialization
		_xdgScreenSaverProcess = new QProcess(QCoreApplication::instance());
	}
	_XWindowID = qApp->activeWindow()->winId();
	QStringList args;
	args << "suspend";
	args << QString::number(_XWindowID);
	int errorCode = _xdgScreenSaverProcess->execute("xdg-screensaver", args);
	TkUtilDebug() << args << errorCode;
	TkUtilDebug() << _xdgScreenSaverProcess->readAll();
#endif	//Q_WS_X11
}

void ScreenSaver::restore() {
	TkUtilDebug();

#ifdef Q_WS_WIN
	if (_stateSaved) {
		SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT, _lowPowerTimeout, NULL, 0);
		SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT, _powerOffTimeout, NULL, 0);
		SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, _screenSaverTimeout, NULL, 0);
	} else {
		TkUtilCritical() << "Screensaver cannot be restored";
	}
#endif	//Q_WS_WIN

#ifdef Q_WS_X11
	if (_XWindowID > 0) {
		QStringList args;
		args << "resume";
		args << QString::number(_XWindowID);
		if (_xdgScreenSaverProcess) {
			int errorCode = _xdgScreenSaverProcess->execute("xdg-screensaver", args);
			TkUtilDebug() << args << errorCode;
			TkUtilDebug() << _xdgScreenSaverProcess->readAll();
		} else {
			TkUtilCritical() << "No xdg-screensaver process";
		}
	} else {
		TkUtilCritical() << "_XWindowID cannot be 0";
	}
#endif	//Q_WS_X11
}
