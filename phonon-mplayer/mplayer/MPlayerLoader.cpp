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

MPlayerLoader::MPlayerLoader(MPlayerProcess * process, QObject * parent)
	: QObject(parent) {

	_process = process;
}

MPlayerLoader::~MPlayerLoader() {
}

void MPlayerLoader::startMPlayerProcess(const QString & filename, int videoWidgetId) {
	_currentProcess = _process;

	_process->clearArguments();

	_process->addArgument(MPLAYER_EXE);

	_process->addArgument("-noquiet");

	//No fullscreen mode
	_process->addArgument("-nofs");

	//TODO ?
	_process->addArgument("-identify");

	//Slave mode
	_process->addArgument("-slave");

	//Video output
	_process->addArgument("-vo");
#ifdef Q_OS_WIN
	_process->addArgument("directx,");
#else
	_process->addArgument("xv,");
#endif	//Q_OS_WIN

	//TODO ?
	_process->addArgument("-zoom");

	//Attach MPlayer video output to our widget
	_process->addArgument("-wid");
	_process->addArgument(QString::number(videoWidgetId));

	_process->addArgument(filename);

	//Log command
	QString commandLine = _process->arguments().join(" ");
	qDebug() << "MPlayer command:" << commandLine;

	if (!_process->start()) {
		//Error handling
		qCritical() << __FUNCTION__ << "error: MPlayer process couldn't start";
	}
}

MPlayerProcess * MPlayerLoader::getCurrentMPlayerProcess() {
	return _currentProcess;
}
