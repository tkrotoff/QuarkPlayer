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

#include "StatusBar.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

Q_EXPORT_PLUGIN2(statusbar, StatusBarFactory);

PluginInterface * StatusBarFactory::create(QuarkPlayer & quarkPlayer) const {
	return new StatusBar(quarkPlayer);
}

StatusBar::StatusBar(QuarkPlayer & quarkPlayer)
	: QStatusBar(NULL),
	PluginInterface(quarkPlayer) {

	_currentTime = 0;
	_totalTime = 0;

	connect(&(quarkPlayer.currentMediaObject()), SIGNAL(tick(qint64)), SLOT(tick(qint64)));
	connect(&(quarkPlayer.currentMediaObject()), SIGNAL(totalTimeChanged(qint64)), SLOT(totalTimeChanged(qint64)));
	connect(&(quarkPlayer.currentMediaObject()), SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State, Phonon::State)));
	connect(&(quarkPlayer.currentMediaObject()), SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
	connect(&(quarkPlayer.currentMediaObject()), SIGNAL(aboutToFinish()), SLOT(aboutToFinish()));

	//10 seconds before the end
	quarkPlayer.currentMediaObject().setPrefinishMark(10000);
	connect(&(quarkPlayer.currentMediaObject()), SIGNAL(prefinishMarkReached(qint32)), SLOT(prefinishMarkReached(qint32)));

	_timeLabel = new QLabel(this);
	//Text color is white
	_timeLabel->setStyleSheet("color: rgb(255, 255, 255);");
	_timeLabel->setMargin(2);
	_timeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	//Resets timeLabel
	tick(0);

	addPermanentWidget(_timeLabel);

	setSizeGripEnabled(false);

	//Background color is black and text color is white
	setStyleSheet("background-color: rgb(0, 0, 0);color: rgb(255, 255, 255);");

	//Add the statusbar to the main window
	quarkPlayer.mainWindow().setStatusBar(this);
}

StatusBar::~StatusBar() {
}

QString StatusBar::convertMilliseconds(qint64 currentTime, qint64 totalTime) {
	QTime displayCurrentTime((currentTime / 3600000) % 60, (currentTime / 60000) % 60, (currentTime / 1000) % 60);
	QTime displayTotalTime((totalTime / 3600000) % 60, (totalTime / 60000) % 60, (totalTime / 1000) % 60);

	QString timeFormat;

	if (displayTotalTime.hour() == 0 && displayTotalTime.minute() == 0 &&
		displayTotalTime.second() == 0 && displayTotalTime.msec() == 0) {
		//Total time is 0, then only return current time
		if (displayCurrentTime.hour() > 0) {
			timeFormat = "hh:mm:ss";
		} else {
			timeFormat = "mm:ss";
		}
		return displayCurrentTime.toString(timeFormat);
	} else {
		if (displayTotalTime.hour() > 0) {
			timeFormat = "hh:mm:ss";
		} else {
			timeFormat = "mm:ss";
		}
		return displayCurrentTime.toString(timeFormat) + " / " + displayTotalTime.toString(timeFormat);
	}
}

void StatusBar::tick(qint64 time) {
	_currentTime = time;
	_timeLabel->setText(convertMilliseconds(_currentTime, _totalTime));
}

void StatusBar::totalTimeChanged(qint64 totalTime) {
	_totalTime = totalTime;
	_timeLabel->setText(convertMilliseconds(_currentTime, _totalTime));
}

void StatusBar::stateChanged(Phonon::State newState, Phonon::State oldState) {
	switch (newState) {
	case Phonon::ErrorState:
		if (quarkPlayer().currentMediaObject().errorType() == Phonon::FatalError) {
			showMessage("Fatal error:" + quarkPlayer().currentMediaObject().errorString());
		} else {
			showMessage("Error:" + quarkPlayer().currentMediaObject().errorString());
		}
		break;

	case Phonon::PlayingState:
		showMessage("Playing");
		break;

	case Phonon::StoppedState:
		showMessage("Stopped");
		break;

	case Phonon::PausedState:
		showMessage("Paused");
		break;

	case Phonon::LoadingState:
		showMessage("Loading...");
		break;

	case Phonon::BufferingState:
		showMessage("Buffering...");
		break;

	default:
		qCritical() << "Error: unknown newState:" << newState << "oldState:" << oldState;
	}
}

void StatusBar::metaDataChanged() {
	QMultiMap<QString, QString> metaData = quarkPlayer().currentMediaObject().metaData();

	QString title = metaData.value("TITLE");
	if (title.isEmpty()) {
		title = quarkPlayer().currentMediaObject().currentSource().fileName();
	}

	/*titleLabel->setText(title);
	artistLabel->setText(metaData.value("ARTIST"));
	albumLabel->setText(metaData.value("ALBUM"));
	yearLabel->setText(metaData.value("DATE"));*/

	_fileTitle = title;
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
