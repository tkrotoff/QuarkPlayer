/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "MPlayerLoader.h"

#include "MPlayerProcess.h"

#include <QtCore/QtDebug>

MPlayerProcess * MPlayerLoader::_currentProcess = NULL;

#ifdef Q_OS_WIN
	static const char * MPLAYER_EXE = "C:/Program Files/SMPlayer/mplayer/mplayer.exe";
#else
	static const char * MPLAYER_EXE = "mplayer";
#endif

MPlayerLoader::MPlayerLoader(QObject * parent)
	: QObject(parent) {

}

MPlayerLoader::~MPlayerLoader() {
}

MPlayerProcess * MPlayerLoader::createNewMPlayerProcess() {
	MPlayerProcess * process = new MPlayerProcess(this);
	_currentProcess = process;

	_processList << _currentProcess;

	return _currentProcess;
}

MPlayerProcess * MPlayerLoader::startMPlayerProcess(const QString & filename, int videoWidgetId) {
	MPlayerProcess * process = createNewMPlayerProcess();

	QStringList args;
	args << "-noquiet";

	//No fullscreen mode
	args << "-nofs";

	//TODO ?
	args << "-identify";

	//Slave mode
	args << "-slave";

	//Video output
	args << "-vo";
#ifdef Q_OS_WIN
	args << "directx,";
#else
	args << "xv,";
#endif	//Q_OS_WIN

	//TODO ?
	args << "-zoom";

	//Attach MPlayer video output to our widget
	args << "-wid";
	args << QString::number(videoWidgetId);

	args << filename;

	if (!process->start(MPLAYER_EXE, args)) {
		//Error handling
		qCritical() << __FUNCTION__ << "error: MPlayer process couldn't start";
	}

	return process;
}

MPlayerProcess * MPlayerLoader::loadMedia(const QString & filename) {
	MPlayerProcess * process = createNewMPlayerProcess();

	QStringList args;
	args << "-identify";
	args << "-frames";
	args << "0";
	args << filename;

	if (!process->start(MPLAYER_EXE, args)) {
		//Error handling
		qCritical() << __FUNCTION__ << "error: MPlayer process couldn't start";
	}

	return process;
}

MPlayerProcess * MPlayerLoader::getCurrentMPlayerProcess() {
	return _currentProcess;
}
