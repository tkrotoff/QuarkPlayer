/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "PlayToolBar.h"

#include "PlayToolBarLogger.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/PluginManager.h>
#include <quarkplayer/style/QuarkPlayerStyle.h>

#include <quarkplayer-plugins/MainWindow/MainWindow.h>

#include <TkUtil/Actions.h>
#include <TkUtil/LanguageChangeEventFilter.h>

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/volumeslider.h>

#include <QtGui/QtGui>

Q_EXPORT_PLUGIN2(PlayToolBar, PlayToolBarFactory);

const char * PlayToolBarFactory::PLUGIN_NAME = "PlayToolBar";

QStringList PlayToolBarFactory::dependencies() const {
	QStringList tmp;
	tmp += MainWindowFactory::PLUGIN_NAME;
	return tmp;
}

PluginInterface * PlayToolBarFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new PlayToolBar(quarkPlayer, uuid, MainWindowFactory::mainWindow());
}

PlayToolBar::PlayToolBar(QuarkPlayer & quarkPlayer, const QUuid & uuid, IMainWindow * mainWindow)
	: QToolBar(NULL),
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

	//By default QuarkPlayerStyle: specific style for QuarkPlayer
	//Fix some ugly things under Windows XP
	_seekToolBar->setStyle(&QuarkPlayerStyle::instance());
	_controlToolBar->setStyle(&QuarkPlayerStyle::instance());

	addWidget(widget);

	//PlayToolBar is disabled at the beginning
	//it will be enabled when a file is being played
	setToolBarEnabled(false);

	//Add to the main window
	mainWindow->setPlayToolBar(this);

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
	Q_UNUSED(newState);

	static bool firstTime = true;

	if (firstTime) {
		firstTime = false;
		setToolBarEnabled(true);
	}
}

void PlayToolBar::createSeekToolBar() {
	_seekToolBar = new QToolBar(NULL);
	_seekToolBar->setIconSize(QSize(24, 18));

	//SeekSlider
	_seekSlider = new Phonon::SeekSlider();
	//_seekSlider->setIconVisible(true);
	//_seekSlider->setTracking(false);

	//_seekToolBar->addAction(Actions::get("CommonActions.SpeedDecrease10%"));
	connect(Actions::get("CommonActions.SpeedDecrease10%"), SIGNAL(triggered()), SLOT(decreaseSpeed10()));
	//_seekToolBar->addAction(Actions::get("CommonActions.JumpBackward10min"));
	connect(Actions::get("CommonActions.JumpBackward10min"), SIGNAL(triggered()), SLOT(jumpBackward10min()));
	_seekToolBar->addAction(Actions::get("CommonActions.JumpBackward1min"));
	connect(Actions::get("CommonActions.JumpBackward1min"), SIGNAL(triggered()), SLOT(jumpBackward1min()));
	//_seekToolBar->addAction(Actions::get("CommonActions.JumpBackward10s"));
	connect(Actions::get("CommonActions.JumpBackward10s"), SIGNAL(triggered()), SLOT(jumpBackward10s()));

	_seekToolBar->addWidget(_seekSlider);

	//_seekToolBar->addAction(Actions::get("CommonActions.JumpForward10s"));
	connect(Actions::get("CommonActions.JumpForward10s"), SIGNAL(triggered()), SLOT(jumpForward10s()));
	_seekToolBar->addAction(Actions::get("CommonActions.JumpForward1min"));
	connect(Actions::get("CommonActions.JumpForward1min"), SIGNAL(triggered()), SLOT(jumpForward1min()));
	//_seekToolBar->addAction(Actions::get("CommonActions.JumpForward10min"));
	connect(Actions::get("CommonActions.JumpForward10min"), SIGNAL(triggered()), SLOT(jumpForward10min()));
	//_seekToolBar->addAction(Actions::get("CommonActions.SpeedIncrease10%"));
	connect(Actions::get("CommonActions.SpeedIncrease10%"), SIGNAL(triggered()), SLOT(increaseSpeed10()));

	connect(Actions::get("CommonActions.VolumeDecrease10%"), SIGNAL(triggered()), SLOT(volumeDecrease10()));
	connect(Actions::get("CommonActions.VolumeIncrease10%"), SIGNAL(triggered()), SLOT(volumeIncrease10()));
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
	qint64 seek = mediaObject->currentTime() - 1000 * 15;
	if (seek < 0) {
		seek = 0;
	}
	mediaObject->seek(seek);
}

void PlayToolBar::jumpForward10s() {
	Phonon::MediaObject * mediaObject = quarkPlayer().currentMediaObject();
	qint64 seek = mediaObject->currentTime() + 1000 * 15;
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

void PlayToolBar::updateVolumeIcon(qreal volume) {
#ifdef NEW_TITLE_CHAPTER_HANDLING
	if (volume <= 0.0) {
		_volumeSlider->setVolumeIcon(QIcon::fromTheme("audio-volume-muted"));
	} else if (volume < 0.3) {
		_volumeSlider->setVolumeIcon(QIcon::fromTheme("audio-volume-low"));
	} else if (volume > 0.7) {
		_volumeSlider->setVolumeIcon(QIcon::fromTheme("audio-volume-high"));
	} else {
		_volumeSlider->setVolumeIcon(QIcon::fromTheme("audio-volume-medium"));
	}
#else
	Q_UNUSED(volume);
#endif	//NEW_TITLE_CHAPTER_HANDLING
}

void PlayToolBar::createControlToolBar() {
	_controlToolBar = new QToolBar(NULL);
	_controlToolBar->setIconSize(QSize(24, 18));

	_controlToolBar->addAction(Actions::get("CommonActions.PreviousTrack"));
	_controlToolBar->addAction(Actions::get("CommonActions.PlayPause"));
	_controlToolBar->addAction(Actions::get("CommonActions.Stop"));
	_controlToolBar->addAction(Actions::get("CommonActions.NextTrack"));

	_controlToolBar->addSeparator();
	_controlToolBar->addAction(Actions::get("CommonActions.FullScreen"));

	_controlToolBar->addSeparator();
	_controlToolBar->addAction(Actions::get("CommonActions.NewMediaObject"));

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

	//FIXME crash: no MediaObject at this time
	//updateVolumeIcon(quarkPlayer().currentAudioOutput()->volume());

#ifdef NEW_TITLE_CHAPTER_HANDLING
	_volumeSlider->setMutedIcon(QIcon::fromTheme("audio-volume-muted"));
	//_seekSlider->setIcon(QIcon::fromTheme("player-time"));
#endif	//NEW_TITLE_CHAPTER_HANDLING

	setMinimumSize(sizeHint());
}

void PlayToolBar::setToolBarEnabled(bool enabled) {
	//FIXME don't know why, seekToolBar does not get enabled afterwards
	//_seekToolBar->setEnabled(enabled);

	Actions::get("CommonActions.PreviousTrack")->setEnabled(enabled);
	Actions::get("CommonActions.PlayPause")->setEnabled(enabled);
	Actions::get("CommonActions.Stop")->setEnabled(enabled);
	Actions::get("CommonActions.NextTrack")->setEnabled(enabled);
	Actions::get("CommonActions.FullScreen")->setEnabled(enabled);
}

void PlayToolBar::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State)));

	//Update current MediaObject state
	stateChanged(mediaObject->state());

	_seekSlider->setMediaObject(mediaObject);

	Phonon::AudioOutput * audioOutput = quarkPlayer().currentAudioOutput();
	if (audioOutput) {
		_volumeSlider->setAudioOutput(audioOutput);
		disconnect(audioOutput, SIGNAL(volumeChanged(qreal)), this, SLOT(updateVolumeIcon(qreal)));
		connect(audioOutput, SIGNAL(volumeChanged(qreal)), SLOT(updateVolumeIcon(qreal)));

		updateVolumeIcon(audioOutput->volume());
	}
}
