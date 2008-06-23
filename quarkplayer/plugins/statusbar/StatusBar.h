/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <quarkplayer/PluginInterface.h>

#include <phonon/phononnamespace.h>

#include <QtGui/QStatusBar>

class QuarkPlayer;

namespace Phonon {
	class MediaObject;
}

class QLabel;

/**
 * Main window status bar.
 *
 * @author Tanguy Krotoff
 */
class StatusBar : public QStatusBar, public PluginInterface {
	Q_OBJECT
public:

	StatusBar(QuarkPlayer & quarkPlayer);

	~StatusBar();

private slots:

	void tick(qint64 time);

	void totalTimeChanged(qint64 totalTime);

	void stateChanged(Phonon::State newState, Phonon::State oldState);

	void metaDataChanged();

	void aboutToFinish();

	void prefinishMarkReached(qint32 msecToEnd);

private:

	static QString convertMilliseconds(qint64 currentTime, qint64 totalTime);

	QLabel * _timeLabel;

	qint64 _totalTime;

	qint64 _currentTime;

	QString _fileTitle;
};

#include <quarkplayer/PluginFactory.h>

class StatusBarFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	PluginInterface * create(QuarkPlayer & quarkPlayer) const;
};

#endif	//STATUSBAR_H
