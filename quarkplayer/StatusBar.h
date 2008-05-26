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

#include <QtGui/QStatusBar>

namespace Phonon {
	class MediaObject;
	enum State;
}

class QLabel;

/**
 * Main window status bar.
 *
 * @author Tanguy Krotoff
 */
class StatusBar : public QStatusBar {
	Q_OBJECT
public:

	StatusBar(Phonon::MediaObject * mediaObject);

	~StatusBar();

private slots:

	void tick(qint64 time);

	void totalTimeChanged(qint64 totalTime);

	void stateChanged(Phonon::State newState, Phonon::State oldState);

	void metaDataChanged();

private:

	static QString convertMilliseconds(qint64 time);

	Phonon::MediaObject * _mediaObject;

	QLabel * _timeLabel;

	qint64 _totalTime;

	qint64 _currentTime;

	QString _fileTitle;
};

#endif	//STATUSBAR_H
