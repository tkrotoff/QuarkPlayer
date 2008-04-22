/*
 * MultipleVideoPlayer, a simple Phonon player
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include "ui_VideoWindow.h"

#include <phonon/mediaobject.h>
#include <phonon/mediacontroller.h>
#include <phonon/audiooutput.h>
#include <phonon/mediasource.h>
#include <phonon/videowidget.h>
#include <phonon/seekslider.h>
#include <phonon/volumeslider.h>

/**
 *
 *
 * @author Tanguy Krotoff
 */
class VideoWindow : public QMainWindow, public Ui::VideoWindow {
	Q_OBJECT
public:

	VideoWindow(QWidget * parent);

	~VideoWindow();

	Phonon::MediaObject * getMediaObject() const;

private slots:

	void playDVD();

	void aboutToFinish();

	void stateChanged(Phonon::State newState, Phonon::State oldState);
	void tick(qint64 time);
	void totalTimeChanged(qint64 newTotalTime);
	void sourceChanged(const Phonon::MediaSource & source);
	void metaDataChanged();

	void availableAudioChannelsChanged();
	void actionAudioStreamTriggered(int id);

	void availableSubtitlesChanged();
	void actionSubtitleStreamTriggered(int id);

	void availableTitlesChanged();
	void actionTitleTriggered(int id);

	void availableChaptersChanged();
	void actionChapterTriggered(int id);

	void availableAnglesChanged();
	void actionAngleTriggered(int id);

private:

	void closeEvent(QCloseEvent * event);

	Phonon::MediaObject * _mediaObject;
	Phonon::MediaController * _mediaController;

	Phonon::VideoWidget * _videoWidget;
	Phonon::AudioOutput * _audioOutput;
	Phonon::MediaSource * _mediaSource;

	Phonon::SeekSlider * _seekSlider;
	Phonon::VolumeSlider * _volumeSlider;
};

#endif	//VIDEOWINDOW_H
