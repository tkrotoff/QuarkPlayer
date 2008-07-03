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

#include "Track.h"

#include <tkutil/TkFile.h>

#include <phonon/mediasource.h>

#include <QtCore/QUrl>
#include <QtCore/QTime>
#include <QtCore/QDebug>

Track::Track(const Phonon::MediaSource & mediaSource) {
	_source = mediaSource;

	QString shortFilename;
	switch (_source.type()) {
	case Phonon::MediaSource::LocalFile:
		_filename = _source.fileName();
		shortFilename = TkFile::fileName(_filename);
		break;
	case Phonon::MediaSource::Url:
		shortFilename = _filename = _source.url().toString();
		break;
	case Phonon::MediaSource::Disc:
		shortFilename = _filename = _source.deviceName();
		break;
	case Phonon::MediaSource::Invalid: {
		//Try to get the filename from the url
		QUrl url(_source.url());
		if (url.isValid()) {
			if (url.scheme() == "file") {
				_filename = url.toLocalFile();
				shortFilename = TkFile::fileName(_filename);
			} else {
				shortFilename = _filename = url.toString();
			}
		}
		break;
	}
	default:
		qCritical() << __FUNCTION__ << "Error: unknown MediaSource type:" << _source.type();
	}

	//By default, title is just the filename
	_title = shortFilename;
}

Track::Track(const Track & track) {
	copy(track);
}

Track::~Track() {
}

void Track::copy(const Track & track) {
	_source = track._source;

	_filename = track._filename;

	_trackNumber = track._trackNumber;
	_title = track._title;
	_artist = track._artist;
	_album = track._album;
	_length = track._length;
}

Track & Track::operator=(const Track & right) {
	//Handle self-assignment
	if (this == &right) {
		return *this;
	}

	copy(right);
	return *this;
}

int Track::operator==(const Track & right) {
	return _filename == right._filename;
}

QString Track::fileName() const {
	return _filename;
}

Phonon::MediaSource Track::mediaSource() const {
	return _source;
}

void Track::setTrackNumber(const QString & trackNumber) {
	_trackNumber = trackNumber;
}

QString Track::trackNumber() const {
	return _trackNumber;
}

void Track::setTitle(const QString & title) {
	if (title.isEmpty()) {
		//Not the fullpath, only the filename
		_title = TkFile::fileName(_filename);
	} else {
		_title = title;
	}
}

QString Track::title() const {
	return _title;
}

void Track::setArtist(const QString & artist) {
	_artist = artist;
}

QString Track::artist() const {
	return _artist;
}

void Track::setAlbum(const QString & album) {
	_album = album;
}

QString Track::album() const {
	return _album;
}

void Track::setLength(const QString & length) {
	_length = convertMilliseconds(length.toULongLong());
}

QString Track::length() const {
	return _length;
}

QString Track::convertMilliseconds(qint64 totalTime) const {
	QTime displayTotalTime((totalTime / 3600000) % 60, (totalTime / 60000) % 60, (totalTime / 1000) % 60);

	QString timeFormat;

	if (displayTotalTime.hour() == 0 && displayTotalTime.minute() == 0 &&
		displayTotalTime.second() == 0 && displayTotalTime.msec() == 0) {
		//Total time is 0, return nothing
		return QString();
	} else {
		if (displayTotalTime.hour() > 0) {
			timeFormat = "hh:mm:ss";
		} else {
			timeFormat = "mm:ss";
		}
		return displayTotalTime.toString(timeFormat);
	}
}
