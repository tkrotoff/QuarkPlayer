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

#include <filetypes/FileTypes.h>

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
	#include <taglib/asffile.h>
	#include <taglib/mp4file.h>

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

	if (_mediaSource.type() == Phonon::MediaSource::Url) {
		_filename = _mediaSource.url().toString();
		_isUrl = true;

		//Cannot solve meta data from a stream/remote media
		qCritical() << __FUNCTION__ << "Error: mediaSource is a url";
	} else {
		_filename = _mediaSource.fileName();
		_isUrl = false;

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

void MediaInfoFetcher::start(Phonon::MediaObject * mediaObject) {
	clear();

	_mediaSource = mediaObject->currentSource();

	if (_mediaSource.type() == Phonon::MediaSource::Url) {
		_filename = _mediaSource.url().toString();
		_isUrl = true;
	} else {
		_filename = _mediaSource.fileName();
		_isUrl = false;
	}

	//Cannot solve meta data from a stream/remote media if we have only the MediaSource
	//We need the MediaObject
	//Use the given mediaObject to get the meta data
	//So when using MediaInfoFetcher you must check if the source is a url
	//and in this case provide a mediaObject
	//Couldn't find a better solution :/

	//Save _metaObjectInfoResolver
	Phonon::MediaObject * saveMetaObjectInfoResolver = _metaObjectInfoResolver;

	//Use the given mediaObject
	_metaObjectInfoResolver = mediaObject;

	metaStateChanged(Phonon::StoppedState, Phonon::StoppedState);

	//Back to normal
	_metaObjectInfoResolver = saveMetaObjectInfoResolver;
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
	QMap<QString, QString> metaData = _metaObjectInfoResolver->metaData();

	if (newState == Phonon::ErrorState) {
		_fetched = true;
		emit fetched();
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

	if (fileRef.isNull()) {
		qCritical() << __FUNCTION__ << "Error: the FileRef is null:" << _filename;
	} else {

		//File type
		if (TagLib::MPEG::File * file = dynamic_cast<TagLib::MPEG::File *>(fileRef.file())) {
			_fileType = FileTypes::fileType(FileType::MP3);
		} else if (TagLib::Ogg::Vorbis::File * file = dynamic_cast<TagLib::Ogg::Vorbis::File *>(fileRef.file())) {
			_fileType = FileTypes::fileType(FileType::Vorbis);
		} else if (TagLib::Ogg::FLAC::File * file = dynamic_cast<TagLib::Ogg::FLAC::File *>(fileRef.file())) {
			_fileType = FileTypes::fileType(FileType::FLAC);
		} else if (TagLib::Ogg::Speex::File * file = dynamic_cast<TagLib::Ogg::Speex::File *>(fileRef.file())) {
			_fileType = FileTypes::fileType(FileType::Speex);
		} else if (TagLib::MPC::File * file = dynamic_cast<TagLib::MPC::File *>(fileRef.file())) {
			_fileType = FileTypes::fileType(FileType::MPC);
		} else if (TagLib::WavPack::File * file = dynamic_cast<TagLib::WavPack::File *>(fileRef.file())) {
			_fileType = FileTypes::fileType(FileType::WavPack);
		} else if (TagLib::TrueAudio::File * file = dynamic_cast<TagLib::TrueAudio::File *>(fileRef.file())) {
			_fileType = FileTypes::fileType(FileType::TTA);
		} else if (TagLib::ASF::File * file = dynamic_cast<TagLib::ASF::File *>(fileRef.file())) {
			//ASF is a container, WMA is the real codec, see http://en.wikipedia.org/wiki/Advanced_Systems_Format
			//and http://en.wikipedia.org/wiki/Windows_Media_Audio
			//Let's ASF be WMA, more consistent for the users
			_fileType = FileTypes::fileType(FileType::WMA);
		} else if (TagLib::MP4::File * file = dynamic_cast<TagLib::MP4::File *>(fileRef.file())) {
			_fileType = FileTypes::fileType(FileType::MP4);
		}

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
	QString extension(TkFile::fileExtension(_filename));

	_fileType = FileTypes::fileType(extension);
}

bool MediaInfoFetcher::hasBeenFetched() const {
	return _fetched;
}

QString MediaInfoFetcher::fileName() const {
	return _filename;
}

bool MediaInfoFetcher::isUrl() const {
	return _isUrl;
}

FileType MediaInfoFetcher::fileType() const {
	return _fileType;
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
	QString tmp;

	if (!_isUrl) {
		//Switch from bytes to megabytes
		QFile file(_filename);
		qint64 fileSize = file.size();
		if (fileSize > 0) {
			tmp.sprintf("%.3f", fileSize / 1000000.0);
			tmp.replace(".", ",");
		}
	}

	return tmp;
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
