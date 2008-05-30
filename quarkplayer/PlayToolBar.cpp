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

#include "PlayToolBar.h"

#include "ui_PlayToolBar.h"

#include "MainWindow.h"

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/volumeslider.h>

#include <QtGui/QtGui>

PlayToolBar::PlayToolBar(Phonon::MediaObject * mediaObject, Phonon::AudioOutput * audioOutput)
	: QToolBar("playToolBar", NULL) {

	_mediaObject = mediaObject;
	_audioOutput = audioOutput;
	_volumeSlider = NULL;
	_seekSlider = NULL;

	QVBoxLayout * vLayout = new QVBoxLayout();
	vLayout->setSpacing(0);
	vLayout->setMargin(0);
	QWidget * widget = new QWidget();
	widget->setLayout(vLayout);

	QToolBar * seekToolBar = createSeekToolBar();
	vLayout->addWidget(seekToolBar);

	QToolBar * controlToolBar = createControlToolBar();
	vLayout->addWidget(controlToolBar);

	//Actions connect
	connect(_ui->actionPlay, SIGNAL(triggered()), _mediaObject, SLOT(play()));
	connect(_ui->actionPause, SIGNAL(triggered()), _mediaObject, SLOT(pause()));
	connect(_ui->actionStop, SIGNAL(triggered()), _mediaObject, SLOT(stop()));
	connect(_ui->actionFullScreen, SIGNAL(toggled(bool)), SIGNAL(fullScreenButtonClicked(bool)));

	addWidget(widget);

	//setMovable(false);
	//setFloatable(false);
}

PlayToolBar::~PlayToolBar() {
	delete _ui;
}

void PlayToolBar::setCheckedFullScreenButton(bool checked) {
	_ui->actionFullScreen->setChecked(checked);
}

void PlayToolBar::stateChanged(Phonon::State newState, Phonon::State oldState) {
	switch (newState) {
	case Phonon::ErrorState:
		break;

	case Phonon::PlayingState:
		_ui->actionPlay->setEnabled(false);
		_ui->actionPause->setEnabled(true);
		_ui->actionStop->setEnabled(true);
		break;

	case Phonon::StoppedState:
		_ui->actionStop->setEnabled(false);
		_ui->actionPlay->setEnabled(true);
		_ui->actionPause->setEnabled(false);
		break;

	case Phonon::PausedState:
		_ui->actionPause->setEnabled(false);
		_ui->actionStop->setEnabled(true);
		_ui->actionPlay->setEnabled(true);
		break;

	case Phonon::LoadingState:
		break;

	case Phonon::BufferingState:
		break;

	default:
		qDebug() << "State? newState=" << newState << "oldState=" << oldState;
		;
	}
}

void PlayToolBar::showOver(QWidget * widgetUnder) {
	resize(widgetUnder->width(), height());

	int x = 0;
	int y = widgetUnder->height() - height();

	move(x, y);
	show();
}

QToolBar * PlayToolBar::createSeekToolBar() {
	QToolBar * seekToolBar = new QToolBar();

	//SeekSlider
	_seekSlider = new Phonon::SeekSlider();
	_seekSlider->setIconVisible(false);
	//_seekSlider->setTracking(false);
	_seekSlider->setMediaObject(_mediaObject);

	seekToolBar->addWidget(_seekSlider);

	return seekToolBar;
}

QToolBar * PlayToolBar::createControlToolBar() {
	QToolBar * controlToolBar = new QToolBar();

	_ui = new Ui::PlayToolBar();
	_ui->setupUi(controlToolBar);

	//volumdeSlider
	_volumeSlider = new Phonon::VolumeSlider(_audioOutput);
	//volumeSlider only takes the space it needs
	_volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	controlToolBar->addWidget(_volumeSlider);

	return controlToolBar;
}
