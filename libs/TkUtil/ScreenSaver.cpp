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

#include <QtGui/QApplication>

#ifdef Q_WS_WIN
	#include <windows.h>

	/**
	 * QApplication event filter that catches Windows events SC_SCREENSAVE and SC_MONITORPOWER.
	 */
	bool myScreenSaverEventFilter(void * message, long * result) {
		MSG * msg = static_cast<MSG *>(message);

		if (msg && msg->message == WM_SYSCOMMAND) {
			if (msg->wParam == SC_SCREENSAVE || msg->wParam == SC_MONITORPOWER) {
				//Intercept ScreenSaver and Monitor Power Messages
				//Prior to activating the screen saver, Windows send this message with the wParam
				//set to SC_SCREENSAVE to all top-level windows. If you set the return value of the
				//message to a non-zero value the screen saver will not start.

				//In fact, because of Qt, we don't care about the result value
				//It works with values 0 & 1
				*result = 1;

				TkUtilDebug() << "Intercept Windows screensaver event";

				//bool QCoreApplication::winEventFilter(MSG * msg, long * result)
				//If you don't want the event to be processed by Qt, then return true and
				//set result to the value that the window procedure should return.
				//Otherwise return false.
				return true;
			}
		}

		return false;
	}
#endif	//Q_WS_WIN

#ifdef Q_WS_X11
	#include <QtGui/QWidget>
	#include <QtCore/QProcess>

	QProcess * _xdgScreenSaverProcess = NULL;
	WId _XWindowID = 0;
#endif	//Q_WS_X11

void ScreenSaver::disable() {
	TkUtilDebug();

	QApplication * app = qobject_cast<QApplication *>(QApplication::instance());
	Q_ASSERT(app);

#ifdef Q_WS_WIN
	//restore() will set the event filter to NULL
	app->setEventFilter(myScreenSaverEventFilter);
#endif	//Q_WS_WIN

#ifdef Q_WS_X11
	if (!_xdgScreenSaverProcess) {
		//Lazy initialization
		_xdgScreenSaverProcess = new QProcess(app);
	}
	_XWindowID = app->activeWindow()->winId();
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

	QApplication * app = qobject_cast<QApplication *>(QApplication::instance());
	Q_ASSERT(app);

#ifdef Q_WS_WIN
	app->setEventFilter(NULL);
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
