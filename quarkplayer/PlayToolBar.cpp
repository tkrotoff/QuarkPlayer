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

#include "ActionCollection.h"
#include "MyIcon.h"

#include "MainWindow.h"

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/volumeslider.h>

#include <QtGui/QtGui>

PlayToolBar::PlayToolBar(Phonon::MediaObject * mediaObject, Phonon::AudioOutput * audioOutput)
	: QToolBar("Play ToolBar", NULL) {

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
	ActionCollection::action("actionFullScreen")->setChecked(checked);
}

void PlayToolBar::stateChanged(Phonon::State newState, Phonon::State oldState) {
	switch (newState) {
	case Phonon::ErrorState:
		break;

	case Phonon::PlayingState:
		ActionCollection::action("actionPlay")->setEnabled(false);
		ActionCollection::action("actionPause")->setEnabled(true);
		ActionCollection::action("actionStop")->setEnabled(true);
		break;

	case Phonon::StoppedState:
		ActionCollection::action("actionStop")->setEnabled(false);
		ActionCollection::action("actionPlay")->setEnabled(true);
		ActionCollection::action("actionPause")->setEnabled(false);
		break;

	case Phonon::PausedState:
		ActionCollection::action("actionPause")->setEnabled(false);
		ActionCollection::action("actionStop")->setEnabled(true);
		ActionCollection::action("actionPlay")->setEnabled(true);
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

	controlToolBar->addAction(ActionCollection::action("actionPlay"));
	controlToolBar->addAction(ActionCollection::action("actionPause"));
	controlToolBar->addAction(ActionCollection::action("actionStop"));
	controlToolBar->addSeparator();
	controlToolBar->addAction(ActionCollection::action("actionPreviousTrack"));
	controlToolBar->addAction(ActionCollection::action("actionNextTrack"));
	controlToolBar->addSeparator();
	controlToolBar->addAction(ActionCollection::action("actionFullScreen"));
	controlToolBar->addSeparator();

	//Actions connect
	connect(ActionCollection::action("actionPlay"), SIGNAL(triggered()), _mediaObject, SLOT(play()));
	connect(ActionCollection::action("actionPause"), SIGNAL(triggered()), _mediaObject, SLOT(pause()));
	connect(ActionCollection::action("actionStop"), SIGNAL(triggered()), _mediaObject, SLOT(stop()));
	connect(ActionCollection::action("actionFullScreen"), SIGNAL(toggled(bool)), SIGNAL(fullScreenButtonClicked(bool)));

	//volumdeSlider
	_volumeSlider = new Phonon::VolumeSlider(_audioOutput);
	_volumeSlider->setIconSize(controlToolBar->iconSize());
	_volumeSlider->setVolumeIcon(MyIcon("speaker"));
	_volumeSlider->setMutedIcon(MyIcon("speaker"));
	//volumeSlider only takes the space it needs
	_volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	controlToolBar->addWidget(_volumeSlider);

	return controlToolBar;
}

void PlayToolBar::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();
	QAction * action;

	action = new QAction(app);
	action->setText(tr("Play"));
	action->setIcon(MyIcon("media-playback-start"));
	ActionCollection::addAction("actionPlay", action);

	action = new QAction(app);
	action->setText(tr("Pause"));
	action->setIcon(MyIcon("media-playback-pause"));
	ActionCollection::addAction("actionPause", action);

	action = new QAction(app);
	action->setText(tr("Stop"));
	action->setIcon(MyIcon("media-playback-stop"));
	ActionCollection::addAction("actionStop", action);

	action = new QAction(app);
	action->setText(tr("Next Track"));
	action->setIcon(MyIcon("media-skip-forward"));
	ActionCollection::addAction("actionNextTrack", action);

	action = new QAction(app);
	action->setText(tr("Previous Track"));
	action->setIcon(MyIcon("media-skip-backward"));
	ActionCollection::addAction("actionPreviousTrack", action);

	action = new QAction(app);
	action->setText(tr("FullScreen"));
	action->setIcon(MyIcon("view-fullscreen"));
	action->setCheckable(true);
	ActionCollection::addAction("actionFullScreen", action);
}
