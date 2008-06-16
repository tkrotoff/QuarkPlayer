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

#include <tkutil/ActionCollection.h>
#include <tkutil/TkIcon.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/volumeslider.h>

#include <QtGui/QtGui>

PlayToolBar::PlayToolBar(Phonon::MediaObject * mediaObject, Phonon::AudioOutput * audioOutput)
	: QToolBar(NULL) {

	RETRANSLATE(this);

	populateActionCollection();

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

	addWidget(widget);

	//setMovable(false);
	//setFloatable(false);
}

PlayToolBar::~PlayToolBar() {
}

void PlayToolBar::setCheckedFullScreenButton(bool checked) {
	ActionCollection::action("fullScreen")->setChecked(checked);
}

void PlayToolBar::stateChanged(Phonon::State newState, Phonon::State oldState) {
	switch (newState) {
	case Phonon::ErrorState:
		break;

	case Phonon::PlayingState:
		ActionCollection::action("play")->setEnabled(false);
		ActionCollection::action("pause")->setEnabled(true);
		ActionCollection::action("stop")->setEnabled(true);
		break;

	case Phonon::StoppedState:
		ActionCollection::action("stop")->setEnabled(false);
		ActionCollection::action("play")->setEnabled(true);
		ActionCollection::action("pause")->setEnabled(false);
		break;

	case Phonon::PausedState:
		ActionCollection::action("pause")->setEnabled(false);
		ActionCollection::action("stop")->setEnabled(true);
		ActionCollection::action("play")->setEnabled(true);
		break;

	case Phonon::LoadingState:
		break;

	case Phonon::BufferingState:
		break;

	default:
		qDebug() << "State? newState=" << newState << "oldState=" << oldState;
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
	QToolBar * seekToolBar = new QToolBar(NULL);

	//SeekSlider
	_seekSlider = new Phonon::SeekSlider();
	_seekSlider->setIconVisible(false);
	//_seekSlider->setTracking(false);
	_seekSlider->setMediaObject(_mediaObject);

	seekToolBar->addWidget(_seekSlider);

	return seekToolBar;
}

QToolBar * PlayToolBar::createControlToolBar() {
	QToolBar * controlToolBar = new QToolBar(NULL);

	controlToolBar->addAction(ActionCollection::action("play"));
	controlToolBar->addAction(ActionCollection::action("pause"));
	controlToolBar->addAction(ActionCollection::action("stop"));
	controlToolBar->addSeparator();
	controlToolBar->addAction(ActionCollection::action("previousTrack"));
	controlToolBar->addAction(ActionCollection::action("nextTrack"));
	controlToolBar->addSeparator();
	controlToolBar->addAction(ActionCollection::action("fullScreen"));
	controlToolBar->addSeparator();

	//Actions connect
	connect(ActionCollection::action("play"), SIGNAL(triggered()), _mediaObject, SLOT(play()));
	connect(ActionCollection::action("pause"), SIGNAL(triggered()), _mediaObject, SLOT(pause()));
	connect(ActionCollection::action("stop"), SIGNAL(triggered()), _mediaObject, SLOT(stop()));
	connect(ActionCollection::action("fullScreen"), SIGNAL(toggled(bool)),
		SIGNAL(fullScreenButtonClicked(bool)));

	//volumdeSlider
	_volumeSlider = new Phonon::VolumeSlider(_audioOutput);
	_volumeSlider->setIconSize(controlToolBar->iconSize());
	//volumeSlider only takes the space it needs
	_volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	controlToolBar->addWidget(_volumeSlider);

	return controlToolBar;
}

void PlayToolBar::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("play", new QAction(app));
	ActionCollection::addAction("pause", new QAction(app));
	ActionCollection::addAction("stop", new QAction(app));
	ActionCollection::addAction("nextTrack", new QAction(app));
	ActionCollection::addAction("previousTrack", new QAction(app));

	QAction * action = new QAction(app);
	action->setCheckable(true);
	ActionCollection::addAction("fullScreen", action);
}

void PlayToolBar::retranslate() {
	ActionCollection::action("play")->setText(tr("Play"));
	ActionCollection::action("play")->setIcon(TkIcon("media-playback-start"));

	ActionCollection::action("pause")->setText(tr("Pause"));
	ActionCollection::action("pause")->setIcon(TkIcon("media-playback-pause"));

	ActionCollection::action("stop")->setText(tr("Stop"));
	ActionCollection::action("stop")->setIcon(TkIcon("media-playback-stop"));

	ActionCollection::action("nextTrack")->setText(tr("Next Track"));
	ActionCollection::action("nextTrack")->setIcon(TkIcon("media-skip-forward"));

	ActionCollection::action("previousTrack")->setText(tr("Previous Track"));
	ActionCollection::action("previousTrack")->setIcon(TkIcon("media-skip-backward"));

	ActionCollection::action("fullScreen")->setText(tr("FullScreen"));
	ActionCollection::action("fullScreen")->setIcon(TkIcon("view-fullscreen"));

	setWindowTitle(tr("Play ToolBar"));

	_volumeSlider->setVolumeIcon(TkIcon("speaker"));
	_volumeSlider->setMutedIcon(TkIcon("speaker"));

	setMinimumSize(sizeHint());
}
