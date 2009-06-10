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

#include "TkTime.h"

#include <QtCore/QDebug>
#include <QtCore/QTime>

QString TkTime::convertMilliseconds(qint64 milliseconds) {
	QTime displayTime((milliseconds / 3600 / 1000) % 60,
			(milliseconds / 60 / 1000) % 60,
			(milliseconds / 1000) % 60);

	QString timeFormat;

	if (displayTime.hour() == 0 && displayTime.minute() == 0 &&
		displayTime.second() == 0 && displayTime.msec() == 0) {
		//Total time is 0, return nothing
		return QString();
	} else {
		if (displayTime.hour() > 0) {
			timeFormat = "hh:mm:ss";
		} else {
			timeFormat = "mm:ss";
		}
		return displayTime.toString(timeFormat);
	}
}

QString TkTime::convertSeconds(int seconds) {
	return convertMilliseconds(seconds * 1000);
}

QString TkTime::convertMilliseconds(qint64 currentTime, qint64 totalTime) {
	QTime displayCurrentTime((currentTime / 3600 / 1000) % 60,
			(currentTime / 60 / 1000) % 60,
			(currentTime / 1000) % 60);
	QTime displayTotalTime((totalTime / 3600 / 1000) % 60,
			(totalTime / 60 / 1000) % 60,
			(totalTime / 1000) % 60);

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
