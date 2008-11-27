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

#include <QtCore/QString>
#include <QtCore/QStringList>

class MediaSettings;
class MPlayerProcess;

class QObject;

/**
 * Helps to launch a MPlayer process.
 *
 * Acts like a simple proxy.
 * This class might become more "intelligent" in the future.
 *
 * @author Tanguy Krotoff
 */
class MPlayerLoader {
public:

	static MediaSettings settings;

	static MPlayerProcess * createNewMPlayerProcess(QObject * parent);

	/**
	 * Starts an empty MPlayerProcess in order to get MPlayer version number.
	 *
	 * Using the MPlayer version number, we performs some tests (see MPlayerLoader.cpp)
	 * and thus we need to know the MPlayer version number as soon as possible.
	 */
	static void startMPlayerVersion(QObject * parent);

	static void loadMedia(MPlayerProcess * process, const QString & filename);

	static void start(MPlayerProcess * process, const QString & filename, int videoWidgetId, qint64 seek = 0);

	static void restart(MPlayerProcess * process, const QStringList & arguments = QStringList(), const QString & filename = QString(), qint64 seek = -1);

private:

	MPlayerLoader();
	~MPlayerLoader();

	/**
	 * Reads the media settings to use them as arguments for the MPlayer process.
	 *
	 * @see http://www.mplayerhq.hu/DOCS/man/en/mplayer.1.html for a complete list of MPlayer parameters
	 * @return list of arguments for the MPlayer process
	 */
	static QStringList readMediaSettings();
};

#endif	//MPLAYERLOADER_H
