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

#include <tkutil/TkFile.h>

#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QDebug>

MediaInfoFetcher::MediaInfoFetcher(QObject * parent)
	: QObject(parent) {

	_isUrl = false;
	_fetched = false;
	_metaObjectInfoResolver = NULL;
}

MediaInfoFetcher::~MediaInfoFetcher() {
}

void MediaInfoFetcher::start(const Phonon::MediaSource & mediaSource) {
	_fetched = false;
	_mediaSource = mediaSource;

	if (mediaSource.type() == Phonon::MediaSource::Url) {
		_filename = mediaSource.url().toString();
		_isUrl = true;
	} else {
		_filename = mediaSource.fileName();
		_isUrl = false;
	}

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
	}

	_metaObjectInfoResolver->setCurrentSource(_mediaSource);
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
		_streamUrl = metaData.value("STREAM_URL");

		_fetched = true;
		emit fetched();
	}
}

void MediaInfoFetcher::startTagLibResolver() {
}

bool MediaInfoFetcher::hasBeenFetched() const {
	return _fetched;
}

QString MediaInfoFetcher::filename() const {
	return _filename;
}

bool MediaInfoFetcher::isUrl() const {
	return _isUrl;
}

QString MediaInfoFetcher::trackNumber() const {
	if (_trackNumber > 0) {
		return QString::number(_trackNumber);
	} else {
		return QString();
	}
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

QString MediaInfoFetcher::year() const {
	if (_year > 0) {
		return QString::number(_year);
	} else {
		return QString();
	}
}

QString MediaInfoFetcher::genre() const {
	return _genre;
}

QString MediaInfoFetcher::comment() const {
	return _comment;
}

QString MediaInfoFetcher::length() const {
	if (_length > 0) {
		return QString::number(_length);
	} else {
		return QString();
	}
}

QString MediaInfoFetcher::bitrate() const {
	if (_bitrate > 0) {
		return QString::number(_bitrate / 1000.0);
	} else {
		return QString();
	}
}

QString MediaInfoFetcher::fileSize() {
	if (!_isUrl) {
		//Switch from bytes to megabytes
		QFile file(_filename);
		QString tmp;
		tmp.sprintf("%.3f", file.size() / 1000000.0);
		tmp.replace(".", ",");
		return tmp;
	} else {
		return QString();
	}
}

QString MediaInfoFetcher::channels() const {
	if (_channels > 0) {
		return QString::number(_channels);
	} else {
		return QString();
	}
}

QString MediaInfoFetcher::sampleRate() const {
	if (_sampleRate > 0) {
		return QString::number(_sampleRate);
	} else {
		return QString();
	}
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

QString MediaInfoFetcher::streamUrl() const {
	return _streamUrl;
}
