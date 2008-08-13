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

#include "config.h"

#include <tkutil/TkFile.h>
#include <tkutil/TkTime.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#ifdef TAGLIB
	#include <taglib/fileref.h>
	#include <taglib/tag.h>
	#include <taglib/audioproperties.h>

	#include <taglib/mpegfile.h>
	#include <taglib/mpcfile.h>
	#include <taglib/mpegfile.h>
	#include <taglib/oggfile.h>
	#include <taglib/oggflacfile.h>
	#include <taglib/speexfile.h>
	#include <taglib/vorbisfile.h>
	#include <taglib/wavpackfile.h>
	#include <taglib/trueaudiofile.h>
	#include <taglib/flacfile.h>

	#include <taglib/apetag.h>
	#include <taglib/id3v2tag.h>
	#include <taglib/id3v1tag.h>
	#include <taglib/xiphcomment.h>

	/**
	 * Converts a Qt4 QString to a TagLib::String.
	 */
	#define Qt4QStringToTString(s) TagLib::String(s.toUtf8().data(), TagLib::String::UTF8)
#endif	//TAGLIB

#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QDebug>

MediaInfoFetcher::MediaInfoFetcher(QObject * parent)
	: QObject(parent) {

	_metaObjectInfoResolver = NULL;

	clear();
}

MediaInfoFetcher::~MediaInfoFetcher() {
}

void MediaInfoFetcher::clear() {
	_fetched = false;

	_isUrl = false;

	_fileType = Unknown;

	_trackNumber = -1;
	_title.clear();
	_artist.clear();
	_album.clear();
	_year = -1;
	_genre.clear();
	_comment.clear();

	_length = -1;
	_bitrate = -1;
	_sampleRate = -1;
	_channels = -1;

	_streamName.clear();
	_streamGenre.clear();
	_streamWebsite.clear();
	_streamUrl.clear();
}

void MediaInfoFetcher::start(const Phonon::MediaSource & mediaSource) {
	clear();

	_mediaSource = mediaSource;

	if (mediaSource.type() == Phonon::MediaSource::Url) {
		_filename = mediaSource.url().toString();
		_isUrl = true;
	} else {
		_filename = mediaSource.fileName();
		_isUrl = false;
	}

	if (_isUrl) {
		startPhononResolver();
	} else {
		determineFileTypeFromExtension();
#ifdef TAGLIB
		//Use TagLib only for files on the harddrive, not for URLs
		//See http://article.gmane.org/gmane.comp.kde.devel.taglib/864
		//Run it inside a different thread since TagLib is a synchronous library
		QtConcurrent::run(this, &MediaInfoFetcher::startTagLibResolver);
#else
		startPhononResolver();
#endif	//TAGLIB
	}
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

	if (newState == Phonon::ErrorState) {
		_fetched = true;
		emit fetched();
		return;
	}

	if (newState == Phonon::StoppedState) {
		_trackNumber = metaData.value("TRACKNUMBER").toInt();
		_title = metaData.value("TITLE");
		_artist = metaData.value("ARTIST");
		_album = metaData.value("ALBUM");
		_year = metaData.value("DATE").toInt();
		_genre = metaData.value("GENRE");
		_comment = metaData.value("COMMENT");

		//Converts from milliseconds to seconds
		_length = metaData.value("LENGTH").toInt() / 1000.0;
		//Converts from bps to kbps
		_bitrate = metaData.value("BITRATE").toInt() / 1000;
		_sampleRate = metaData.value("SAMPLERATE").toInt();
		_channels = metaData.value("CHANNELS").toInt();

		_streamName = metaData.value("STREAM_NAME");
		_streamGenre = metaData.value("STREAM_GENRE");
		_streamWebsite = metaData.value("STREAM_WEBSITE");
		_streamUrl = metaData.value("STREAM_URL");

		_fetched = true;
		emit fetched();
	}
}

void MediaInfoFetcher::startTagLibResolver() {
#ifdef TAGLIB
	TagLib::String str(Qt4QStringToTString(_filename));
	TagLib::FileRef fileRef(str.toCString());

	bool fileIsNull = fileRef.isNull();

	//File type
	if (TagLib::MPEG::File * file = dynamic_cast<TagLib::MPEG::File *>(fileRef.file())) {
		_fileType = MP3;
	} else if (TagLib::Ogg::Vorbis::File * file = dynamic_cast<TagLib::Ogg::Vorbis::File *>(fileRef.file())) {
		_fileType = Ogg;
	} else if (TagLib::Ogg::FLAC::File * file = dynamic_cast<TagLib::Ogg::FLAC::File *>(fileRef.file())) {
		_fileType = FLAC;
	} else if (TagLib::Ogg::Speex::File * file = dynamic_cast<TagLib::Ogg::Speex::File *>(fileRef.file())) {
		_fileType = Speex;
	} else if (TagLib::MPC::File * file = dynamic_cast<TagLib::MPC::File *>(fileRef.file())) {
		_fileType = MPC;
	} else if (TagLib::WavPack::File * file = dynamic_cast<TagLib::WavPack::File *>(fileRef.file())) {
		_fileType = WavPack;
	} else if (TagLib::TrueAudio::File * file = dynamic_cast<TagLib::TrueAudio::File *>(fileRef.file())) {
		_fileType = TrueAudio;
	}

	//Missing: MP4 .mp4, WMA .wma

	if (fileIsNull) {
		qCritical() << __FUNCTION__ << "Error: the FileRef is null:" << _filename;
	} else {
		TagLib::Tag * tag = fileRef.tag();
		if (tag) {
			if (!tag->isEmpty()) {
				_trackNumber = tag->track();
				_title = TStringToQString(tag->title()).trimmed();
				_artist = TStringToQString(tag->artist()).trimmed();
				_album = TStringToQString(tag->album()).trimmed();
				_year = tag->year();
				_genre = TStringToQString(tag->genre()).trimmed();
				_comment = TStringToQString(tag->comment()).trimmed();
			}
		}

		TagLib::AudioProperties * audioProperties = fileRef.audioProperties();
		if (audioProperties) {
			//Converts to seconds since Phonon gives us milliseconds
			_length = audioProperties->length();
			_bitrate = audioProperties->bitrate();
			_sampleRate = audioProperties->sampleRate();
			_channels = audioProperties->channels();
		}
	}

	_fetched = true;
	emit fetched();
#endif	//TAGLIB
}

void MediaInfoFetcher::determineFileTypeFromExtension() {
	QString ext(TkFile::fileExtension(_filename));

	if (ext == "mp3") {
		_fileType = MP3;
	} else if (ext == ".ogg" || ext == ".oga") {
		_fileType = Ogg;
	} else if (ext == "flac") {
		_fileType = FLAC;
	} else if (ext == "spx") {
		_fileType = Speex;
	} else if (ext == "mpc" || ext == ".mp+" || ext == "mpp") {
		_fileType = MPC;
	} else if (ext == "wv") {
		_fileType = WavPack;
	} else if (ext == "tta") {
		_fileType = TrueAudio;
	} else if (ext == "wma") {
		_fileType = WMA;
	} else if (ext == "mp4") {
		_fileType = MP4;
	} else if (ext == "") {
		_fileType = Unknown;
	} else if (ext == "") {
		_fileType = Unknown;
	} else if (ext == "") {
		_fileType = Unknown;
	} else if (ext == "") {
		_fileType = Unknown;
	} else if (ext == "") {
		_fileType = Unknown;
	} else if (ext == "") {
		_fileType = Unknown;
	} else if (ext == "") {
		_fileType = Unknown;
	} else if (ext == "") {
		_fileType = Unknown;
	} else if (ext == "") {
		_fileType = Unknown;
	}
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

QString MediaInfoFetcher::fileType() const {
	QString type;

	switch (_fileType) {
	case Unknown:
		break;
	case MP3:
		type = "MP3";
		break;
	case Ogg:
		type = "Ogg Vorbis";
		break;
	case WMA:
		type = "WMA";
		break;
	case MP4:
		type = "MP4";
		break;
	case FLAC:
		type = "FLAC";
		break;
	default:
		qCritical() << __FUNCTION__ << "Error: unknown file type:" << _fileType;
	}

	return type;
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
		return TkTime::convertSeconds(_length);
	} else {
		return QString();
	}
}

QString MediaInfoFetcher::bitrate() const {
	if (_bitrate > 0) {
		return QString::number(_bitrate);
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

QString MediaInfoFetcher::channels() const {
	if (_channels > 0) {
		return QString::number(_channels);
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
