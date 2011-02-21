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

#include "StatusBar.h"

#include "StatusBarLogger.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/PluginManager.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/style/QuarkPlayerStyle.h>

#include <quarkplayer-plugins/MainWindow/MainWindow.h>

#include <TkUtil/TkTime.h>
#include <TkUtil/MouseEventFilter.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

static const char * STATUSBAR_TIME_DIPLAY_MODE_KEY = "statusbar_time_display_mode";

Q_EXPORT_PLUGIN2(StatusBar, StatusBarFactory);

const char * StatusBarFactory::PLUGIN_NAME = "StatusBar";

QStringList StatusBarFactory::dependencies() const {
	QStringList tmp;
	tmp += MainWindowFactory::PLUGIN_NAME;
	return tmp;
}

PluginInterface * StatusBarFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new StatusBar(quarkPlayer, uuid, MainWindowFactory::mainWindow());
}

StatusBar::StatusBar(QuarkPlayer & quarkPlayer, const QUuid & uuid, IMainWindow * mainWindow)
	: QStatusBar(mainWindow),
	PluginInterface(quarkPlayer, uuid) {

	Q_ASSERT(mainWindow);
	_mainWindow = mainWindow;

	_blinker = NULL;

	_timeLabel = new QLabel(this);

	//Cursor is a hand to show that the label is clickable
	_timeLabel->setCursor(Qt::PointingHandCursor);

	_timeLabel->setMargin(2);
	_timeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	_timeLabel->installEventFilter(new MousePressEventFilter(this, SLOT(changeTimeDisplayMode())));
	addPermanentWidget(_timeLabel);

	setSizeGripEnabled(false);

	//Background color is black and text color is white
	_backgroundColor = QColor(0, 0, 0);
	_textColor = QColor(255, 255, 255);
	setStyleSheet(QString("background-color: %1; color: %2;")
			.arg(_backgroundColor.name())
			.arg(_textColor.name()));
	changeTimeLabelTextColor(_textColor);

	//By default QuarkPlayerStyle: specific style for QuarkPlayer
	//Fix some ugly things under Windows XP
	setStyle(&QuarkPlayerStyle::instance());

	//Add the statusbar to the main window
	_mainWindow->setStatusBar(this);

	Config::instance().addKey(STATUSBAR_TIME_DIPLAY_MODE_KEY, TimeDisplayModeElapsed);

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));
}

StatusBar::~StatusBar() {
	//Remove the statusbar from the main window
	_mainWindow->setStatusBar(NULL);
}

void StatusBar::tick(qint64 time) {
	QString timeText;

	if (time > 0) {
		qint64 totalTime = quarkPlayer().currentMediaObject()->totalTime();
		TimeDisplayMode timeDisplayMode = static_cast<TimeDisplayMode>(
			Config::instance().value(STATUSBAR_TIME_DIPLAY_MODE_KEY).toInt());

		switch (timeDisplayMode) {
		case TimeDisplayModeElapsed:
			timeText = TkTime::convertMilliseconds(time, totalTime);
			break;
		case TimeDisplayModeRemaining:
			timeText = "- " + TkTime::convertMilliseconds(totalTime - time, totalTime);
			break;
		default:
			StatusBarCritical() << "Unknown TimeDisplayMode:" << timeDisplayMode;
		}
	}

	_timeLabel->setText(timeText);
}

void StatusBar::changeTimeDisplayMode() {
	StatusBarDebug();

	TimeDisplayMode timeDisplayMode = static_cast<TimeDisplayMode>(
		Config::instance().value(STATUSBAR_TIME_DIPLAY_MODE_KEY).toInt());
	TimeDisplayMode newTimeDisplayMode = TimeDisplayModeElapsed;	//Initialization just to avoid a warning
	switch (timeDisplayMode) {
	case TimeDisplayModeElapsed:
		newTimeDisplayMode = TimeDisplayModeRemaining;
		break;
	case TimeDisplayModeRemaining:
		newTimeDisplayMode = TimeDisplayModeElapsed;
		break;
	default:
		StatusBarCritical() << "Unknown TimeDisplayMode:" << timeDisplayMode;
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
			StatusBarCritical() << "Wrong state and error type:" << newState
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
		stopBlinking();
		break;

	case Phonon::StoppedState:
		showMessage(tr("Stopped"));
		//Re-initializes the time label
		tick(0);
		break;

	case Phonon::PausedState:
		showMessage(tr("Paused"));
		startBlinking();
		break;

	case Phonon::LoadingState:
		showMessage(tr("Loading..."));
		break;

	case Phonon::BufferingState:
		showMessage(tr("Buffering..."));
		break;

	default:
		StatusBarCritical() << "Unknown newState:" << newState;
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
	StatusBarDebug();
	showMessage(tr("Media finishing..."));
}

void StatusBar::prefinishMarkReached(qint32 msecToEnd) {
	StatusBarDebug() << msecToEnd;
	showMessage(tr("%1 seconds left...", "", msecToEnd / 1000).arg(msecToEnd / 1000));
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

void StatusBar::startBlinking() {
	if (!_blinker) {
		_blinker = new QTimer(this);
		connect(_blinker, SIGNAL(timeout()), SLOT(blink()));
	}
	_blinker->stop();
	_blinker->start(1000);

	blink(true);
}

void StatusBar::stopBlinking() {
	if (_blinker) {
		_blinker->stop();
	}

	//Text color different from background
	//back to normal text color
	changeTimeLabelTextColor(_textColor);
}

void StatusBar::blink(bool init) {
	static bool hideText = true;

	if (init) {
		hideText = true;
	}

	if (hideText) {
		//Text color same as background
		//so the text is transparent
		changeTimeLabelTextColor(_backgroundColor);
	} else {
		//Text color different from background
		//back to normal text color
		changeTimeLabelTextColor(_textColor);
	}
	hideText = !hideText;
}

void StatusBar::changeTimeLabelTextColor(const QColor & textColor) {
	_timeLabel->setStyleSheet(QString("font: bold; color: %1;").arg(textColor.name()));
}
