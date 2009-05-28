/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

QString PlayToolBarFactory::pluginName() const {
	return "playtoolbar";
}

PluginInterface * PlayToolBarFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new PlayToolBar(quarkPlayer, uuid);
}

PlayToolBar::PlayToolBar(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QToolBar(quarkPlayer.mainWindow()),
	PluginInterface(quarkPlayer, uuid) {

	_volumeSlider = NULL;
	_seekSlider = NULL;

	QVBoxLayout * layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->setContentsMargins(0, 0, 0, 0);
	QWidget * widget = new QWidget();
	widget->setContentsMargins(0, 0, 0, 0);
	widget->setLayout(layout);

	createSeekToolBar();
	layout->addWidget(_seekToolBar);

	createControlToolBar();
	layout->addWidget(_controlToolBar);

	addWidget(widget);

	//PlayToolBar is disabled at the beginning
	//it will be enabled when a file is being played
	setToolBarEnabled(false);

	//Add to the main window
	quarkPlayer.mainWindow()->setPlayToolBar(this);

	if (quarkPlayer.currentMediaObject()) {
		//The current MediaObject has been already created
		//So to wait for the signal is useless: it was already sent long before
		currentMediaObjectChanged(quarkPlayer.currentMediaObject());
	}
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
		ActionCollection::action("MainWindow.FullScreen")->setEnabled(true);
	} else {
		ActionCollection::action("MainWindow.FullScreen")->setEnabled(false);
	}

	switch (newState) {
	case Phonon::ErrorState:
		break;

	case Phonon::PlayingState:
		ActionCollection::action("MainWindow.PlayPause")->setText(tr("&Pause"));
		ActionCollection::action("MainWindow.PlayPause")->setIcon(TkIcon("media-playback-pause"));
		disconnect(ActionCollection::action("MainWindow.PlayPause"), 0, 0, 0);
		connect(ActionCollection::action("MainWindow.PlayPause"), SIGNAL(triggered()),
			quarkPlayer().currentMediaObject(), SLOT(pause()));

		ActionCollection::action("MainWindow.Stop")->setEnabled(true);
		break;

	case Phonon::StoppedState:
		ActionCollection::action("MainWindow.PlayPause")->setText(tr("P&lay"));
		ActionCollection::action("MainWindow.PlayPause")->setIcon(TkIcon("media-playback-start"));
		disconnect(ActionCollection::action("MainWindow.PlayPause"), 0, 0, 0);
		connect(ActionCollection::action("MainWindow.PlayPause"), SIGNAL(triggered()),
			quarkPlayer().currentMediaObject(), SLOT(play()));

		ActionCollection::action("MainWindow.Stop")->setEnabled(false);
		break;

	case Phonon::PausedState:
		ActionCollection::action("MainWindow.PlayPause")->setText(tr("P&lay"));
		ActionCollection::action("MainWindow.PlayPause")->setIcon(TkIcon("media-playback-start"));
		disconnect(ActionCollection::action("MainWindow.PlayPause"), 0, 0, 0);
		connect(ActionCollection::action("MainWindow.PlayPause"), SIGNAL(triggered()),
			quarkPlayer().currentMediaObject(), SLOT(play()));

		ActionCollection::action("MainWindow.Stop")->setEnabled(true);
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
	_seekToolBar->setIconSize(QSize(24, 18));

	//SeekSlider
	_seekSlider = new Phonon::SeekSlider();
	//_seekSlider->setIconVisible(true);
	//_seekSlider->setTracking(false);

	//_seekToolBar->addAction(ActionCollection::action("MainWindow.SpeedDecrease10%"));
	connect(ActionCollection::action("MainWindow.SpeedDecrease10%"), SIGNAL(triggered()), SLOT(decreaseSpeed10()));
	//_seekToolBar->addAction(ActionCollection::action("MainWindow.JumpBackward10min"));
	connect(ActionCollection::action("MainWindow.JumpBackward10min"), SIGNAL(triggered()), SLOT(jumpBackward10min()));
	_seekToolBar->addAction(ActionCollection::action("MainWindow.JumpBackward1min"));
	connect(ActionCollection::action("MainWindow.JumpBackward1min"), SIGNAL(triggered()), SLOT(jumpBackward1min()));
	//_seekToolBar->addAction(ActionCollection::action("MainWindow.JumpBackward10s"));
	connect(ActionCollection::action("MainWindow.JumpBackward10s"), SIGNAL(triggered()), SLOT(jumpBackward10s()));

	_seekToolBar->addWidget(_seekSlider);

	//_seekToolBar->addAction(ActionCollection::action("MainWindow.JumpForward10s"));
	connect(ActionCollection::action("MainWindow.JumpForward10s"), SIGNAL(triggered()), SLOT(jumpForward10s()));
	_seekToolBar->addAction(ActionCollection::action("MainWindow.JumpForward1min"));
	connect(ActionCollection::action("MainWindow.JumpForward1min"), SIGNAL(triggered()), SLOT(jumpForward1min()));
	//_seekToolBar->addAction(ActionCollection::action("MainWindow.JumpForward10min"));
	connect(ActionCollection::action("MainWindow.JumpForward10min"), SIGNAL(triggered()), SLOT(jumpForward10min()));
	//_seekToolBar->addAction(ActionCollection::action("MainWindow.SpeedIncrease10%"));
	connect(ActionCollection::action("MainWindow.SpeedIncrease10%"), SIGNAL(triggered()), SLOT(increaseSpeed10()));

	connect(ActionCollection::action("MainWindow.VolumeDecrease10%"), SIGNAL(triggered()), SLOT(volumeDecrease10()));
	connect(ActionCollection::action("MainWindow.VolumeIncrease10%"), SIGNAL(triggered()), SLOT(volumeIncrease10()));
}

void PlayToolBar::decreaseSpeed10() {
}

void PlayToolBar::jumpBackward10min() {
	Phonon::MediaObject * mediaObject = quarkPlayer().currentMediaObject();
	qint64 seek = mediaObject->currentTime() - 1000 * 60 * 10;
	if (seek < 0) {
		seek = 0;
	}
	mediaObject->seek(seek);
}

void PlayToolBar::jumpBackward1min() {
	Phonon::MediaObject * mediaObject = quarkPlayer().currentMediaObject();
	qint64 seek = mediaObject->currentTime() - 1000 * 60;
	if (seek < 0) {
		seek = 0;
	}
	mediaObject->seek(seek);
}

void PlayToolBar::jumpBackward10s() {
	Phonon::MediaObject * mediaObject = quarkPlayer().currentMediaObject();
	qint64 seek = mediaObject->currentTime() - 1000 * 10;
	if (seek < 0) {
		seek = 0;
	}
	mediaObject->seek(seek);
}

void PlayToolBar::jumpForward10s() {
	Phonon::MediaObject * mediaObject = quarkPlayer().currentMediaObject();
	qint64 seek = mediaObject->currentTime() + 1000 * 10;
	if (seek > mediaObject->totalTime()) {
		seek = mediaObject->totalTime();
	}
	mediaObject->seek(seek);
}

void PlayToolBar::jumpForward1min() {
	Phonon::MediaObject * mediaObject = quarkPlayer().currentMediaObject();
	qint64 seek = mediaObject->currentTime() + 1000 * 60;
	if (seek > mediaObject->totalTime()) {
		seek = mediaObject->totalTime();
	}
	mediaObject->seek(seek);
}

void PlayToolBar::jumpForward10min() {
	Phonon::MediaObject * mediaObject = quarkPlayer().currentMediaObject();
	qint64 seek = mediaObject->currentTime() + 1000 * 60 * 10;
	if (seek > mediaObject->totalTime()) {
		seek = mediaObject->totalTime();
	}
	mediaObject->seek(seek);
}

void PlayToolBar::increaseSpeed10() {
}

void PlayToolBar::volumeDecrease10() {
	Phonon::AudioOutput * audioOutput = quarkPlayer().currentAudioOutput();
	qreal volume = audioOutput->volume() - 0.1;
	if (volume < 0.0) {
		volume = 0.0;
	}
	audioOutput->setVolume(volume);
}

void PlayToolBar::volumeIncrease10() {
	Phonon::AudioOutput * audioOutput = quarkPlayer().currentAudioOutput();
	qreal volume = audioOutput->volume() + 0.1;
	if (volume > 1.0) {
		volume = 1.0;
	}
	audioOutput->setVolume(volume);
}

void PlayToolBar::volumeChanged(qreal volume) {
	if (volume <= 0.0) {
		_volumeSlider->setVolumeIcon(TkIcon("audio-volume-muted"));
	} else if (volume < 0.3) {
		_volumeSlider->setVolumeIcon(TkIcon("audio-volume-low"));
	} else if (volume > 0.7) {
		_volumeSlider->setVolumeIcon(TkIcon("audio-volume-high"));
	} else {
		_volumeSlider->setVolumeIcon(TkIcon("audio-volume-medium"));
	}
}

void PlayToolBar::createControlToolBar() {
	_controlToolBar = new QToolBar(NULL);
	_controlToolBar->setIconSize(QSize(24, 18));

	_controlToolBar->addAction(ActionCollection::action("MainWindow.PreviousTrack"));
	_controlToolBar->addAction(ActionCollection::action("MainWindow.PlayPause"));
	_controlToolBar->addAction(ActionCollection::action("MainWindow.Stop"));
	_controlToolBar->addAction(ActionCollection::action("MainWindow.NextTrack"));

	_controlToolBar->addSeparator();
	_controlToolBar->addAction(ActionCollection::action("MainWindow.FullScreen"));

	_controlToolBar->addSeparator();
	_controlToolBar->addAction(ActionCollection::action("MainWindow.NewMediaObject"));

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
	_volumeSlider->setMutedIcon(TkIcon("audio-volume-muted"));

	//_seekSlider->setIcon(TkIcon("player-time"));

	setMinimumSize(sizeHint());
}

void PlayToolBar::setToolBarEnabled(bool enabled) {
	//FIXME don't why, seekToolBar does not get enabled afterwards
	//_seekToolBar->setEnabled(enabled);

	ActionCollection::action("MainWindow.PreviousTrack")->setEnabled(enabled);
	ActionCollection::action("MainWindow.PlayPause")->setEnabled(enabled);
	ActionCollection::action("MainWindow.Stop")->setEnabled(enabled);
	ActionCollection::action("MainWindow.NextTrack")->setEnabled(enabled);
	ActionCollection::action("MainWindow.FullScreen")->setEnabled(enabled);
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
	disconnect(ActionCollection::action("MainWindow.Stop"), 0, 0, 0);
	connect(ActionCollection::action("MainWindow.Stop"), SIGNAL(triggered()),
		mediaObject, SLOT(stop()));

	_seekSlider->setMediaObject(mediaObject);

	Phonon::AudioOutput * audioOutput = quarkPlayer().currentAudioOutput();
	_volumeSlider->setAudioOutput(quarkPlayer().currentAudioOutput());
	disconnect(audioOutput, SIGNAL(volumeChanged(qreal)), this, SLOT(volumeChanged(qreal)));
	connect(audioOutput, SIGNAL(volumeChanged(qreal)), SLOT(volumeChanged(qreal)));
}
