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
#include <tkutil/TkTime.h>

#include <phonon/mediasource.h>

#include <QtCore/QUrl>
#include <QtCore/QDebug>

Track::Track(const QString & filename) {
	_filename = filename;

	//This avoid a stupid bug: comparing a filename with \ separator and another with /
	//By replacing any \ by /, when don't have any comparison problem
	_filename.replace("\\", "/");

	//Optimization: don't do that because it is slow
	//to construct a Phonon::MediaSource
	//setMediaSource(filename);
	///

	_resolved = false;
}

void Track::setMediaSource(const Phonon::MediaSource & mediaSource) {
	QString shortFilename;
	switch (mediaSource.type()) {
	case Phonon::MediaSource::LocalFile:
		_filename = mediaSource.fileName();
		shortFilename = TkFile::dir(_filename) + "/";
		shortFilename += TkFile::removeFileExtension(TkFile::fileName(_filename));
		break;
	case Phonon::MediaSource::Url:
		shortFilename = _filename = mediaSource.url().toString();
		break;
	case Phonon::MediaSource::Disc:
		shortFilename = _filename = mediaSource.deviceName();
		break;
	case Phonon::MediaSource::Invalid: {
		//Try to get the filename from the url
		QUrl url(mediaSource.url());
		if (url.isValid()) {
			if (url.scheme() == "file") {
				_filename = url.toLocalFile();
				shortFilename = TkFile::dir(_filename) + "/";
				shortFilename += TkFile::removeFileExtension(TkFile::fileName(_filename));
			} else {
				shortFilename = _filename = url.toString();
			}
		}
		break;
	}
	default:
		qCritical() << __FUNCTION__ << "Error: unknown MediaSource type:" << mediaSource.type();
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
	_filename = track._filename;

	_trackNumber = track._trackNumber;
	_title = track._title;
	_artist = track._artist;
	_album = track._album;
	_length = track._length;
	_resolved = track._resolved;
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
	return Phonon::MediaSource(_filename);
}

void Track::setTrackNumber(const QString & trackNumber) {
	_trackNumber = trackNumber;
}

QString Track::trackNumber() const {
	return _trackNumber;
}

void Track::setTitle(const QString & title) {
	if (title.isEmpty()) {
		//Not the fullpath, only the filename + parent directory name
		_title = TkFile::dir(_filename) + "/";
		_title += TkFile::removeFileExtension(TkFile::fileName(_filename));
	} else {
		_title = title;
	}
}

QString Track::title() {
	if (_title.isEmpty()) {
		Phonon::MediaSource source(_filename);
		setMediaSource(source);
	}

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
	_length = length;
}

QString Track::length() const {
	return _length;
}

void Track::setMediaDataResolved(bool resolved) {
	_resolved = resolved;
}

bool Track::mediaDataResolved() const {
	return _resolved;
}
