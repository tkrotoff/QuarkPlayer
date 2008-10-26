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

#include <quarkplayer/MainWindow.h>
#include <quarkplayer/QuarkPlayer.h>

#include <tkutil/ActionCollection.h>
#include <tkutil/TkIcon.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/volumeslider.h>

#include <QtGui/QtGui>

Q_EXPORT_PLUGIN2(playtoolbar, PlayToolBarFactory);

PluginInterface * PlayToolBarFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new PlayToolBar(quarkPlayer, uuid);
}

PlayToolBar::PlayToolBar(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QToolBar(NULL),
	PluginInterface(quarkPlayer, uuid) {

	_volumeSlider = NULL;
	_seekSlider = NULL;

	QVBoxLayout * vLayout = new QVBoxLayout();
	vLayout->setSpacing(0);
	vLayout->setMargin(0);
	QWidget * widget = new QWidget();
	widget->setLayout(vLayout);

	createSeekToolBar();
	vLayout->addWidget(_seekToolBar);

	createControlToolBar();
	vLayout->addWidget(_controlToolBar);

	addWidget(widget);

	//PlayToolBar is disabled at the beginning
	//it will be enabled when a file is being played
	setToolBarEnabled(false);

	//Add to the main window
	quarkPlayer.mainWindow()->setPlayToolBar(this);

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	RETRANSLATE(this);
	retranslate();
}

PlayToolBar::~PlayToolBar() {
}

void PlayToolBar::stateChanged(Phonon::State newState) {
	static bool firstTime = true;

	if (firstTime) {
		firstTime = false;
		setToolBarEnabled(true);
	}

	//Enabled/disabled fullscreen button depending if media file is a video or audio
	if (quarkPlayer().currentMediaObject()->hasVideo()) {
		ActionCollection::action("fullScreen")->setEnabled(true);
	} else {
		ActionCollection::action("fullScreen")->setEnabled(false);
	}

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
		qDebug() << "State? newState=" << newState;
	}
}

void PlayToolBar::createSeekToolBar() {
	_seekToolBar = new QToolBar(NULL);
	//_seekToolBar->setIconSize(QSize(16, 16));

	//SeekSlider
	_seekSlider = new Phonon::SeekSlider();
	_seekSlider->setIconVisible(true);
	//_seekSlider->setTracking(false);

	_seekToolBar->addWidget(_seekSlider);
}

void PlayToolBar::createControlToolBar() {
	_controlToolBar = new QToolBar(NULL);
	//_controlToolBar->setIconSize(QSize(16, 16));

	_controlToolBar->addAction(ActionCollection::action("play"));
	_controlToolBar->addAction(ActionCollection::action("pause"));
	_controlToolBar->addAction(ActionCollection::action("stop"));
	_controlToolBar->addAction(ActionCollection::action("previousTrack"));
	_controlToolBar->addAction(ActionCollection::action("nextTrack"));

	_controlToolBar->addSeparator();
	_controlToolBar->addAction(ActionCollection::action("fullScreen"));

	_controlToolBar->addSeparator();
	_controlToolBar->addAction(ActionCollection::action("newMediaObject"));

	//volumeSlider
	_controlToolBar->addSeparator();
	_volumeSlider = new Phonon::VolumeSlider();
	_volumeSlider->setIconSize(_controlToolBar->iconSize());
	//volumeSlider only takes the space it needs
	_volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	_controlToolBar->addWidget(_volumeSlider);
}

void PlayToolBar::retranslate() {
	setWindowTitle(tr("Play ToolBar"));

	_volumeSlider->setVolumeIcon(TkIcon("player-volume"));
	_volumeSlider->setMutedIcon(TkIcon("player-volume-muted"));

	_seekSlider->setIcon(TkIcon("player-time"));

	setMinimumSize(sizeHint());
}

void PlayToolBar::setToolBarEnabled(bool enabled) {
	//FIXME don't why, seekToolBar does not get enabled afterwards
	//_seekToolBar->setEnabled(enabled);

	ActionCollection::action("play")->setEnabled(enabled);
	ActionCollection::action("pause")->setEnabled(enabled);
	ActionCollection::action("stop")->setEnabled(enabled);
	ActionCollection::action("nextTrack")->setEnabled(enabled);
	ActionCollection::action("previousTrack")->setEnabled(enabled);
	ActionCollection::action("fullScreen")->setEnabled(enabled);
}

void PlayToolBar::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State)));

	//Update current MediaObject state
	stateChanged(mediaObject->state());

	//Actions connect
	disconnect(ActionCollection::action("play"), 0, 0, 0);
	connect(ActionCollection::action("play"), SIGNAL(triggered()),
		mediaObject, SLOT(play()));

	disconnect(ActionCollection::action("pause"), 0, 0, 0);
	connect(ActionCollection::action("pause"), SIGNAL(triggered()),
		mediaObject, SLOT(pause()));

	disconnect(ActionCollection::action("stop"), 0, 0, 0);
	connect(ActionCollection::action("stop"), SIGNAL(triggered()),
		mediaObject, SLOT(stop()));

	_seekSlider->setMediaObject(mediaObject);
	_volumeSlider->setAudioOutput(quarkPlayer().currentAudioOutput());
}
