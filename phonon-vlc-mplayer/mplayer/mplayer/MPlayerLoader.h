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

#include "MediaSettings.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>

class MPlayerProcess;

/**
 * Helps to launch a MPlayer process.
 *
 * Keeps a list of all the MPlayer process.
 *
 * @author Tanguy Krotoff
 */
class MPlayerLoader : public QObject {
	Q_OBJECT
public:

	MediaSettings _settings;

	MPlayerLoader(QObject * parent);
	~MPlayerLoader();

	MPlayerProcess * loadMedia(const QString & filename);

	MPlayerProcess * startMPlayerProcess(const QString & filename, int videoWidgetId, double seek = 0);

	void restartMPlayerProcess(MPlayerProcess * process);

signals:

private slots:

private:

	MPlayerProcess * createNewMPlayerProcess();

	QStringList readMediaSettings() const;

	/** List of MPlayer process. */
	QList<MPlayerProcess *> _processList;

	/** Current MPlayer process. */
	//MPlayerProcess * _currentProcess;
};

#endif	//MPLAYERLOADER_H
