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

#ifndef MPLAYERLOADER_H
#define MPLAYERLOADER_H

#include <QtCore/QObject>
#include <QtCore/QString>

class MPlayerProcess;

#ifdef Q_OS_WIN
	static const char * MPLAYER_EXE = "C:/Program Files/SMPlayer/mplayer/mplayer.exe";
#else
	static const char * MPLAYER_EXE = "mplayer";
#endif

class MPlayerLoader : public QObject {
	Q_OBJECT
public:

	MPlayerLoader(MPlayerProcess * process, QObject * parent);
	~MPlayerLoader();

	void startMPlayerProcess(const QString & filename, int videoWidgetId);

	static MPlayerProcess * getCurrentMPlayerProcess();

private:

	/** MPlayer process. */
	MPlayerProcess * _process;

	static MPlayerProcess * _currentProcess;
};

#endif	//MPLAYERLOADER_H
