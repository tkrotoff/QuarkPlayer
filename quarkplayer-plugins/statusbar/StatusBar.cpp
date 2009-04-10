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

#include "StatusBar.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/config/Config.h>

#include <tkutil/TkTime.h>
#include <tkutil/MouseEventFilter.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

static const char * STATUSBAR_TIME_DIPLAY_MODE_KEY = "statusbar_time_display_mode";

Q_EXPORT_PLUGIN2(statusbar, StatusBarFactory);

PluginInterface * StatusBarFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new StatusBar(quarkPlayer, uuid);
}

StatusBar::StatusBar(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QStatusBar(quarkPlayer.mainWindow()),
	PluginInterface(quarkPlayer, uuid) {

	_timeLabel = new QLabel(this);
	//Text color is white
	_timeLabel->setStyleSheet("color: rgb(255, 255, 255);");
	//Cursor is a hand to show that the label is clickable
	_timeLabel->setCursor(Qt::PointingHandCursor);
	_timeLabel->setMargin(2);
	_timeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	_timeLabel->installEventFilter(new MousePressEventFilter(this, SLOT(changeTimeDisplayMode())));
	addPermanentWidget(_timeLabel);

	setSizeGripEnabled(false);

	//Background color is black and text color is white
	setStyleSheet("background-color: rgb(0, 0, 0);color: rgb(255, 255, 255);");

	//Add the statusbar to the main window
	quarkPlayer.mainWindow()->setStatusBar(this);

	Config::instance().addKey(STATUSBAR_TIME_DIPLAY_MODE_KEY, TimeDisplayModeElapsed);

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));
}

StatusBar::~StatusBar() {
	//Remove the statusbar from the main window
	quarkPlayer().mainWindow()->setStatusBar(NULL);
}

void StatusBar::tick(qint64 time) {
	QString timeText;
	if (time != 0) {
		qint64 totalTime = quarkPlayer().currentMediaObject()->totalTime();
		TimeDisplayMode timeDisplayMode = static_cast<TimeDisplayMode>(Config::instance().value(STATUSBAR_TIME_DIPLAY_MODE_KEY).toInt());

		switch (timeDisplayMode) {
		case TimeDisplayModeElapsed:
			timeText = TkTime::convertMilliseconds(time, totalTime);
			break;
		case TimeDisplayModeRemaining:
			timeText = "- " + TkTime::convertMilliseconds(totalTime - time, totalTime);
			break;
		default:
			qCritical() << __FUNCTION__ << "Error: unknown TimeDisplayMode:" << timeDisplayMode;
		}
	}
	_timeLabel->setText(timeText);
}

void StatusBar::changeTimeDisplayMode() {
	qDebug() << __FUNCTION__;

	TimeDisplayMode timeDisplayMode = static_cast<TimeDisplayMode>(Config::instance().value(STATUSBAR_TIME_DIPLAY_MODE_KEY).toInt());
	TimeDisplayMode newTimeDisplayMode = TimeDisplayModeElapsed;	//Initialization just to avoid a warning
	switch (timeDisplayMode) {
	case TimeDisplayModeElapsed:
		newTimeDisplayMode = TimeDisplayModeRemaining;
		break;
	case TimeDisplayModeRemaining:
		newTimeDisplayMode = TimeDisplayModeElapsed;
		break;
	default:
		qCritical() << __FUNCTION__ << "Error: unknown TimeDisplayMode:" << timeDisplayMode;
	}
	Config::instance().setValue(STATUSBAR_TIME_DIPLAY_MODE_KEY, newTimeDisplayMode);

	tick(quarkPlayer().currentMediaObject()->currentTime());
}

void StatusBar::stateChanged(Phonon::State newState) {
	switch (newState) {

	case Phonon::ErrorState: {
		Phonon::ErrorType errorType = quarkPlayer().currentMediaObject()->errorType();
		QString errorString = quarkPlayer().currentMediaObject()->errorString();
		switch (errorType) {
		case Phonon::NoError:
			//Cannot be in this state
			qCritical() << __FUNCTION__ << "Error: wrong state and error type:" << newState
						<< errorType << errorString;
			break;
		case Phonon::NormalError:
			showMessage("Error: " + errorString);
			break;
		case Phonon::FatalError:
			showMessage("Fatal error: " + errorString);
			break;
		}
		break;
	}

	case Phonon::PlayingState:
		showMessage(tr("Playing"));
		break;

	case Phonon::StoppedState:
		showMessage(tr("Stopped"));
		break;

	case Phonon::PausedState:
		showMessage(tr("Paused"));
		break;

	case Phonon::LoadingState:
		showMessage(tr("Loading..."));
		break;

	case Phonon::BufferingState:
		showMessage(tr("Buffering..."));
		break;

	default:
		qCritical() << "Error: unknown newState:" << newState;
	}
}

void StatusBar::bufferStatus(int percentFilled) {
	showMessage(tr("Buffering... %1%").arg(percentFilled));
}

void StatusBar::finished() {
	showMessage(tr("End of Media"));
}

void StatusBar::showTitle() {
	QString title = quarkPlayer().currentMediaObjectTitle();
	showMessage(title);
}

void StatusBar::aboutToFinish() {
	qDebug() << __FUNCTION__;
	showMessage(tr("Media finishing..."));
}

void StatusBar::prefinishMarkReached(qint32 msecToEnd) {
	qDebug() << __FUNCTION__ << msecToEnd;
	showMessage(tr("%1 seconds left...").arg(msecToEnd / 1000));
}

void StatusBar::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	//Update title since we changed from a MediaObject to another
	showTitle();

	//Update current MediaObject state
	stateChanged(mediaObject->state());

	//Resets current and total time display
	tick(mediaObject->currentTime());

	connect(mediaObject, SIGNAL(tick(qint64)), SLOT(tick(qint64)));
	connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State)));
	connect(mediaObject, SIGNAL(finished()), SLOT(finished()));
	connect(mediaObject, SIGNAL(metaDataChanged()), SLOT(showTitle()));
	connect(mediaObject, SIGNAL(aboutToFinish()), SLOT(aboutToFinish()));
	connect(mediaObject, SIGNAL(bufferStatus(int)), SLOT(bufferStatus(int)));

	//10 seconds before the end
	mediaObject->setPrefinishMark(10000);
	connect(mediaObject, SIGNAL(prefinishMarkReached(qint32)), SLOT(prefinishMarkReached(qint32)));
}
