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

#include <QtCore/QDebug>

Track::Track() {
}

Track::~Track() {
}

void Track::setTrackNumber(const QString & trackNumber) {
	_trackNumber = trackNumber;
}

QString Track::trackNumber() const {
	return _trackNumber;
}

void Track::setTitle(const QString & title) {
	_title = title;
}

QString Track::title() {
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
