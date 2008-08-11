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

#include "MediaInfoFetcher.h"

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtCore/QDebug>

MediaInfoFetcher::MediaInfoFetcher(QObject * parent)
	: QObject(parent) {
}

MediaInfoFetcher::~MediaInfoFetcher() {
}

void MediaInfoFetcher::start(const QString & filename) {
	_fetched = false;
	_filename = filename;
	_metaObjectInfoResolver = NULL;

#ifdef TAGLIB
	startTagLibResolver();
#endif	//TAGLIB

	startPhononResolver();
}

void MediaInfoFetcher::startPhononResolver() {
	if (!_metaObjectInfoResolver) {
		//Lazy initialization
		//Info resolver
		_metaObjectInfoResolver = new Phonon::MediaObject(this);
		connect(_metaObjectInfoResolver, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
			SLOT(metaStateChanged(Phonon::State, Phonon::State)));
		_metaObjectInfoResolver->setCurrentSource(_filename);
	}
}

void MediaInfoFetcher::metaStateChanged(Phonon::State newState, Phonon::State oldState) {
	Phonon::MediaSource source = _metaObjectInfoResolver->currentSource();
	QMap<QString, QString> metaData = _metaObjectInfoResolver->metaData();

	if (source.type() == Phonon::MediaSource::Invalid) {
		return;
	}

	if (newState != Phonon::ErrorState && newState != Phonon::StoppedState) {
		return;
	}

	if (newState == Phonon::StoppedState) {
		_trackNumber = metaData.value("TRACKNUMBER").toInt();
		_title = metaData.value("TITLE");
		_artist = metaData.value("ARTIST");
		_album = metaData.value("ALBUM");
		_length = metaData.value("LENGTH").toInt();
		_bitrate = metaData.value("BITRATE").toInt();

		_streamName = metaData.value("STREAM_NAME");
		_streamGenre = metaData.value("STREAM_GENRE");
		_streamWebsite = metaData.value("STREAM_WEBSITE");
		_streamURL = metaData.value("STREAM_URL");

		_fetched = true;
		emit fetched();
	}
}

void MediaInfoFetcher::startTagLibResolver() {
}

bool MediaInfoFetcher::fetched() const {
	return _fetched;
}

int MediaInfoFetcher::trackNumber() const {
	return _trackNumber;
}

QString MediaInfoFetcher::title() const {
	return _title;
}

QString MediaInfoFetcher::artist() const {
	return _artist;
}

QString MediaInfoFetcher::album() const {
	return _album;
}

int MediaInfoFetcher::year() const {
	return _year;
}

QString MediaInfoFetcher::genre() const {
	return _genre;
}

QString MediaInfoFetcher::comment() const {
	return _comment;
}

int MediaInfoFetcher::length() const {
	return _length;
}

int MediaInfoFetcher::bitrate() const {
	return _bitrate;
}

long MediaInfoFetcher::fileSize() const {
	return _fileSize;
}

int MediaInfoFetcher::channels() const {
	return _channels;
}

int MediaInfoFetcher::sampleRate() const {
	return _sampleRate;
}

QString MediaInfoFetcher::streamName() const {
	return _streamName;
}

QString MediaInfoFetcher::streamGenre() const {
	return _streamGenre;
}

QString MediaInfoFetcher::streamWebsite() const {
	return _streamWebsite;
}

QString MediaInfoFetcher::streamURL() const {
	return _streamURL;
}
