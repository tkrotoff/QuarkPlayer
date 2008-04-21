/*
 * VLC and MPlayer backends for the Phonon library
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "TrackData.h"

#include <QtCore/QFileInfo>

TrackData::TrackData() {
	_lang = "";
	_name = "";
	_filename = "";
	_duration = 0;
	_ID = -1;
	_chapters = 0;
	_angles = 0;
}

TrackData::~TrackData() {
}

QString TrackData::displayName() const {
	//qDebug("TrackData::displayName");

	QString dname;

	if (!_name.isEmpty()) {
		dname = _name;
		if (!_lang.isEmpty()) {
			dname += " ["+ _lang + "]";
		}
	}
	else
	if (!_lang.isEmpty()) {
		dname = _lang;
	}
	else
	if (!_filename.isEmpty()) {
		QFileInfo f(_filename);
		dname = f.fileName();
	}
	else
		dname = QString::number(_ID);

	if (_duration > 0) {
		dname += " ("+ formatTime( (int) _duration ) +")";
	}

	return dname;
}

QString TrackData::formatTime(int secs) {
	int t = secs;
	int hours = (int) t / 3600;
	t -= hours*3600;
	int minutes = (int) t / 60;
	t -= minutes*60;
	int seconds = t;

	QString tf;
	return tf.sprintf("%02d:%02d:%02d",hours,minutes,seconds);
}
