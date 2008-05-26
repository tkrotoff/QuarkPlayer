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

#ifndef PLAYTOOLBAR_H
#define PLAYTOOLBAR_H

#include <phonon/phononnamespace.h>

#include <QtGui/QToolBar>

class MainWindow;

namespace Ui { class PlayToolBar; }

namespace Phonon {
	class MediaObject;
	class AudioOutput;
	class SeekSlider;
	class VolumeSlider;
}

class QLabel;
class QSize;

/**
 * Toolbar containing play/pause/stop...
 *
 * @author Tanguy Krotoff
 */
class PlayToolBar : public QToolBar {
	Q_OBJECT
public:

	PlayToolBar(MainWindow * parent, Phonon::MediaObject * mediaObject, Phonon::AudioOutput * audioOutput);

	~PlayToolBar();

	void setCheckedFullScreenButton(bool checked);

	void showOver(QWidget * widgetUnder);

signals:

	void fullScreenButtonClicked(bool checked);

private slots:

	void stateChanged(Phonon::State newState, Phonon::State oldState);

private:

	QToolBar * createSeekToolBar();

	QToolBar * createControlToolBar();

	Ui::PlayToolBar * _ui;

	Phonon::MediaObject * _mediaObject;

	Phonon::AudioOutput * _audioOutput;

	Phonon::SeekSlider * _seekSlider;

	Phonon::VolumeSlider * _volumeSlider;
};

#endif	//PLAYTOOLBAR_H
