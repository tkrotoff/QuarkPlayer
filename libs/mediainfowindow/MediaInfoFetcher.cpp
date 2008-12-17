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
	#include <taglib/textidentificationframe.h>
	#include <taglib/urllinkframe.h>
	#include <taglib/id3v1tag.h>
	#include <taglib/xiphcomment.h>
#endif	//TAGLIB

#ifdef MEDIAINFOLIB
	#include <MediaInfo/MediaInfoDLL.h>
#endif	//MEDIAINFOLIB

#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QDebug>

#include <iostream>
#include <iomanip>
#include <string>

MediaInfoFetcher::MediaInfoFetcher(QObject * parent)
	: QObject(parent) {

	_metaObjectInfoResolver = NULL;

	clear();
}

MediaInfoFetcher::~MediaInfoFetcher() {
}

void MediaInfoFetcher::clear() {
	_fetched = false;

	//General
	_filename.clear();
	_isUrl = false;
	_fileSize = -1;
	_length = -1;
	_encodedApplication.clear();

	//metaData
	_metadataHash.clear();

	//Audio
	_audioStreamCount = 0;
	_audioStreamHash.clear();

	//Video
	_videoStreamCount = 0;
	_videoStreamHash.clear();

	//Text
	_textStreamCount = 0;
	_textStreamHash.clear();

	//Stream
	_networkStreamHash.clear();
}

void MediaInfoFetcher::start(const Phonon::MediaSource & mediaSource, ReadStyle readStyle) {
	clear();

	_mediaSource = mediaSource;
	_readStyle = readStyle;

	if (_mediaSource.type() == Phonon::MediaSource::Url) {
		_filename = _mediaSource.url().toString();
		_isUrl = true;

		//Cannot solve meta data from a stream/remote media
		qCritical() << __FUNCTION__ << "Error: mediaSource is a url";

		//This might be caused also by an invalid filename

		//Sends the fetched() signal
		_fetched = true;
		emit fetched();
	} else {
		_filename = _mediaSource.fileName();
		_isUrl = false;

		determineFileTypeFromExtension();

		bool resolverLaunched = false;

#ifdef MEDIAINFOLIB
		if (_readStyle == ReadStyleAccurate) {
			QtConcurrent::run(this, &MediaInfoFetcher::startMediaInfoLibResolver);
			resolverLaunched = true;
		}
#endif	//MEDIAINFOLIB

#ifdef TAGLIB
		if (!resolverLaunched) {
			//Use TagLib only for files on the harddrive, not for URLs
			//See http://article.gmane.org/gmane.comp.kde.devel.taglib/864
			//Run it inside a different thread since TagLib is a synchronous library
			QtConcurrent::run(this, &MediaInfoFetcher::startTagLibResolver);
			resolverLaunched = true;
		}
#endif	//TAGLIB

		if (!resolverLaunched) {
			//If TagLib or MediaInfoLib are not used, let's use
			//the backend for resolving the metadata
			startPhononResolver();
			resolverLaunched = true;
		}
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
		insertMetadata(TrackNumber, metaData.value("TRACKNUMBER").trimmed());
		insertMetadata(Title, metaData.value("TITLE").trimmed());
		insertMetadata(Artist, metaData.value("ARTIST").trimmed());
		insertMetadata(Album, metaData.value("ALBUM").trimmed());
		insertMetadata(Year, metaData.value("DATE").trimmed());
		insertMetadata(Genre, metaData.value("GENRE").trimmed());
		insertMetadata(Comment, metaData.value("COMMENT").trimmed());

		//Converts from milliseconds to seconds
		_length = metaData.value("LENGTH").toInt() / 1000.0;
		//Converts from bps to kbps
		insertAudioStream(0, AudioBitrate, QString::number(metaData.value("BITRATE").trimmed().toInt() / 1000));
		insertAudioStream(0, AudioSampleRate, metaData.value("SAMPLERATE").trimmed());
		insertAudioStream(0, AudioChannelCount, metaData.value("CHANNELS").trimmed());

		insertNetworkStream(StreamName, metaData.value("STREAM_NAME").trimmed());
		insertNetworkStream(StreamGenre, metaData.value("STREAM_GENRE").trimmed());
		insertNetworkStream(StreamWebsite, metaData.value("STREAM_WEBSITE").trimmed());
		insertNetworkStream(StreamURL, metaData.value("STREAM_URL").trimmed());

		_fetched = true;
		emit fetched();
	}
}

QString parseID3v2_TXXX(const TagLib::ID3v2::FrameListMap & metadata, const QString & tagName) {
	QString tmp;
	TagLib::ID3v2::FrameList frameList = metadata["TXXX"];
	for (TagLib::ID3v2::FrameList::Iterator it = frameList.begin(); it != frameList.end(); it++) {
		TagLib::ID3v2::TextIdentificationFrame * textFrame = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame *>(*it);
		TagLib::StringList fieldList = textFrame->fieldList();
		if (TStringToQString(fieldList.front()) == tagName) {
			tmp = TStringToQString(fieldList.back()).trimmed();
			break;
		}
	}
	return tmp;
}

QString parseID3v2_WXXX(const TagLib::ID3v2::FrameListMap & metadata) {
	QString tmp;
	TagLib::ID3v2::FrameList frameList = metadata["WXXX"];
	for (TagLib::ID3v2::FrameList::Iterator it = frameList.begin(); it != frameList.end(); it++) {
		TagLib::ID3v2::UrlLinkFrame * urlFrame = dynamic_cast<TagLib::ID3v2::UrlLinkFrame *>(*it);
		tmp = TStringToQString(urlFrame->url()).trimmed();
		if (!tmp.isEmpty()) {
			break;
		}
	}
	return tmp;
}

void MediaInfoFetcher::startTagLibResolver() {
#ifdef TAGLIB
	TagLib::AudioProperties::ReadStyle readStyle = TagLib::AudioProperties::Average;
	switch (_readStyle) {
	case ReadStyleFast:
		readStyle = TagLib::AudioProperties::Fast;
		break;
	case ReadStyleAverage:
		readStyle = TagLib::AudioProperties::Average;
		break;
	case ReadStyleAccurate:
		readStyle = TagLib::AudioProperties::Accurate;
		break;
	default:
		qCritical() << "Error: unknown ReadStyle:" << _readStyle;
	}

	//Taken from Amarok, file: CollectionScanner.cpp
#ifdef Q_OS_WIN
	const wchar_t * encodedName = reinterpret_cast<const wchar_t *>(_filename.utf16());
#else
	const char * encodedName = QFile::encodeName(_filename).constData();
#endif	//Q_OS_WIN

	TagLib::FileRef fileRef(encodedName, true, readStyle);

	if (fileRef.isNull()) {
		qCritical() << __FUNCTION__ << "Error: the FileRef is null:" << _filename;
	} else {

		if (TagLib::MPEG::File * file = dynamic_cast<TagLib::MPEG::File *>(fileRef.file())) {
			_fileType = FileTypes::fileType(FileType::MP3);
			if (file->ID3v2Tag()) {
				TagLib::ID3v2::FrameListMap metadata = file->ID3v2Tag()->frameListMap();

				//4 letters: id3v2.3.0 and id3v2.4.0 (November 1, 2000)
				//3 letters: id3v2.2.0, "considered obsolete" cf http://en.wikipedia.org/wiki/ID3

				if (!metadata["TPOS"].isEmpty()) {
					insertMetadata(DiscNumber, TStringToQString(metadata["TPOS"].front()->toString()).trimmed());
				}
				if (!metadata["TBP"].isEmpty()) {
					insertMetadata(BPM, TStringToQString(metadata["TBP"].front()->toString()).trimmed());
				}
				if (!metadata["TBPM"].isEmpty()) {
					insertMetadata(BPM, TStringToQString(metadata["TBPM"].front()->toString()).trimmed());
				}
				if (!metadata["TCM"].isEmpty()) {
					insertMetadata(Composer, TStringToQString(metadata["TCM"].front()->toString()).trimmed());
				}
				if (!metadata["TCOM"].isEmpty()) {
					insertMetadata(Composer, TStringToQString(metadata["TCOM"].front()->toString()).trimmed());
				}
				if (!metadata["TPE2"].isEmpty()) {
					//Non-standard: Apple, Microsoft
					insertMetadata(AlbumArtist, TStringToQString(metadata["TPE2"].front()->toString()).trimmed());
				}
				if (!metadata["TCMP"].isEmpty()) {
					//TODO
					qDebug() << "Compilation:" << TStringToQString(metadata["TCMP"].front()->toString()).trimmed();
				}
				if (!metadata["TPB"].isEmpty()) {
					insertMetadata(Publisher, TStringToQString(metadata["TPB"].front()->toString()).trimmed());
				}
				if (!metadata["TPUB"].isEmpty()) {
					insertMetadata(Publisher, TStringToQString(metadata["TPUB"].front()->toString()).trimmed());
				}
				if (!metadata["TCR"].isEmpty()) {
					insertMetadata(Copyright, TStringToQString(metadata["TCR"].front()->toString()).trimmed());
				}
				if (!metadata["TCOP"].isEmpty()) {
					insertMetadata(Copyright, TStringToQString(metadata["TCOP"].front()->toString()).trimmed());
				}
				if (!metadata["TEN"].isEmpty()) {
					insertMetadata(EncodedBy, TStringToQString(metadata["TEN"].front()->toString()).trimmed());
				}
				if (!metadata["TENC"].isEmpty()) {
					insertMetadata(EncodedBy, TStringToQString(metadata["TENC"].front()->toString()).trimmed());
				}
				if (!metadata["TOA"].isEmpty()) {
					insertMetadata(OriginalArtist, TStringToQString(metadata["TOA"].front()->toString()).trimmed());
				}
				if (!metadata["TOPE"].isEmpty()) {
					insertMetadata(OriginalArtist, TStringToQString(metadata["TOPE"].front()->toString()).trimmed());
				}

				insertMetadata(URL, parseID3v2_WXXX(metadata));

				insertMetadata(AlbumArtistSort, parseID3v2_TXXX(metadata, "ALBUMARTISTSORT"));

				insertMetadata(MusicBrainzArtistId, parseID3v2_TXXX(metadata, "MusicBrainz Artist Id"));
				insertMetadata(MusicBrainzReleaseId, parseID3v2_TXXX(metadata, "MusicBrainz Album Id"));
				insertMetadata(MusicBrainzTrackId, parseID3v2_TXXX(metadata, "MusicBrainz Track Id"));
				insertMetadata(AmazonId, parseID3v2_TXXX(metadata, "ASIN"));
			}

		} else if (TagLib::Ogg::Vorbis::File * file = dynamic_cast<TagLib::Ogg::Vorbis::File *>(fileRef.file())) {
			_fileType = FileTypes::fileType(FileType::Vorbis);
			if (file->tag()) {
				TagLib::Ogg::FieldListMap metadata = file->tag()->fieldListMap();
				if (!metadata["COMPOSER"].isEmpty()) {
					insertMetadata(Composer, TStringToQString(metadata["COMPOSER"].front()).trimmed());
				}
				if (!metadata["BPM"].isEmpty()) {
					insertMetadata(BPM, TStringToQString(metadata["BPM"].front()).trimmed());
				}
				if (!metadata["DISCNUMBER"].isEmpty()) {
					insertMetadata(DiscNumber, TStringToQString(metadata["DISCNUMBER"].front()).trimmed());
				}
				if (!metadata["COMPILATION"].isEmpty()) {
					//TODO
					qDebug() << "Compilation:" << TStringToQString(metadata["COMPILATION"].front()).trimmed();
				}
			}

		} else if (TagLib::Ogg::FLAC::File * file = dynamic_cast<TagLib::Ogg::FLAC::File *>(fileRef.file())) {
			_fileType = FileTypes::fileType(FileType::FLAC);
			if (file->tag()) {
				TagLib::Ogg::FieldListMap metadata = file->tag()->fieldListMap();
				if (!metadata["COMPOSER"].isEmpty()) {
					insertMetadata(Composer, TStringToQString(metadata["COMPOSER"].front()).trimmed());
				}
				if (!metadata["BPM"].isEmpty()) {
					insertMetadata(BPM, TStringToQString(metadata["BPM"].front()).trimmed());
				}
				if (!metadata["DISCNUMBER"].isEmpty()) {
					insertMetadata(DiscNumber, TStringToQString(metadata["DISCNUMBER"].front()).trimmed());
				}
				if (!metadata["COMPILATION"].isEmpty()) {
					//TODO
					qDebug() << "Compilation:" << TStringToQString(metadata["COMPILATION"].front()).trimmed();
				}
			}

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
			//MP4 is in fact AAC (.m4a), see http://en.wikipedia.org/wiki/Advanced_Audio_Coding
			//and http://en.wikipedia.org/wiki/MPEG-4_Part_14
			_fileType = FileTypes::fileType(FileType::AAC);
		}


		_fileSize = fileRef.file()->length();
		TagLib::Tag * tag = fileRef.tag();
		if (tag) {
			if (!tag->isEmpty()) {
				insertMetadata(TrackNumber, QString::number(tag->track()));
				insertMetadata(Title, TStringToQString(tag->title()).trimmed());
				insertMetadata(Artist, TStringToQString(tag->artist()).trimmed());
				insertMetadata(Album, TStringToQString(tag->album()).trimmed());
				insertMetadata(Year, QString::number(tag->year()));
				insertMetadata(Genre, TStringToQString(tag->genre()).trimmed());
				insertMetadata(Comment, TStringToQString(tag->comment()).trimmed());
			}
		}

		TagLib::AudioProperties * audioProperties = fileRef.audioProperties();
		if (audioProperties) {
			_length = audioProperties->length();
			insertAudioStream(0, AudioBitrate, QString::number(audioProperties->bitrate()));
			insertAudioStream(0, AudioSampleRate, QString::number(audioProperties->sampleRate()));
			insertAudioStream(0, AudioChannelCount, QString::number(audioProperties->channels()));
		}
	}

	_fetched = true;
	emit fetched();
#endif	//TAGLIB
}

void MediaInfoFetcher::startMediaInfoLibResolver() {
#ifdef MEDIAINFOLIB
	MediaInfoLib::MediaInfo mediaInfo;
	QString mediaInfoLibVersion = QString::fromStdWString(mediaInfo.Option(_T("Info_Version"), _T("")));

	//Test if MediaInfoLib is OK
	if (mediaInfoLibVersion.contains("Unable to load")) {
		//Unable to load mediainfo.dll
		//Let's go back to TagLib
		startTagLibResolver();
		return;
	}

	mediaInfo.Open(_filename.toStdWString());

	//Info_Parameters: gets all usefull MediaInfoLib parameters names
	//mediaInfo.Option(_T("Info_Parameters"));
	//mediaInfo.Option(_T("Complete"), _T("1"));
	//qDebug() << QString::fromStdWString(mediaInfo.Inform());

	//General
	_fileSize = QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("FileSize"))).toInt();
	_length = QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Duration"))).toInt() / 1000;
	_bitrate = QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("OverallBitRate"))).toInt() / 1000;
	_encodedApplication = QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Encoded_Application")));

	//Metadata
	insertMetadata(Title, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Title"))).trimmed());
	insertMetadata(Artist, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Performer"))).trimmed());
	insertMetadata(Album, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Album"))).trimmed());
	insertMetadata(AlbumArtist, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Accompaniment"))).trimmed());
	insertMetadata(TrackNumber, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Track/Position"))).trimmed());
	insertMetadata(DiscNumber, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Part/Position"))).trimmed());
	insertMetadata(OriginalArtist, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Original/Performer"))).trimmed());
	insertMetadata(Composer, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Composer"))).trimmed());
	insertMetadata(Publisher, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Publisher"))).trimmed());
	insertMetadata(Genre, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Genre"))).trimmed());
	insertMetadata(Year, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Recorded_Date"))).trimmed());
	insertMetadata(BPM, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("BPM"))).trimmed());
	insertMetadata(Copyright, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Copyright"))).trimmed());
	insertMetadata(Comment, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Comment"))).trimmed());
	insertMetadata(URL, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("URL"))).trimmed());
	insertMetadata(EncodedBy, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Encoded_Library/String"))).trimmed());
	insertMetadata(MusicBrainzArtistId, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("MusicBrainz Artist Id"))).trimmed());
	insertMetadata(MusicBrainzReleaseId, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("MusicBrainz Album Id"))).trimmed());
	insertMetadata(MusicBrainzTrackId, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("MusicBrainz Track Id"))).trimmed());
	insertMetadata(AmazonId, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("ASIN"))).trimmed());

	//Audio
	_audioStreamCount = mediaInfo.Count_Get(MediaInfoLib::Stream_Audio);
	for (int audioStreamId = 0; audioStreamId < _audioStreamCount; audioStreamId++) {
		insertAudioStream(audioStreamId, AudioBitrate, QString::number(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("BitRate"))).trimmed().toInt() / 1000));
		insertAudioStream(audioStreamId, AudioBitrateMode, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("BitRate_Mode"))).trimmed());
		insertAudioStream(audioStreamId, AudioSampleRate, QString::number(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("SamplingRate"))).trimmed().toFloat() / 1000.0));
		insertAudioStream(audioStreamId, AudioBitsPerSample, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Resolution"))).trimmed());
		insertAudioStream(audioStreamId, AudioChannelCount, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Channel(s)"))).trimmed());
		insertAudioStream(audioStreamId, AudioCodec, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Codec/String"))).trimmed());
		insertAudioStream(audioStreamId, AudioCodecProfile, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Codec_Profile"))).trimmed());
		insertAudioStream(audioStreamId, AudioLanguage, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Language/String"))).trimmed());
		insertAudioStream(audioStreamId, AudioEncodedLibrary, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Encoded_Library/String"))).trimmed());
	}

	//Video
	_videoStreamCount = mediaInfo.Count_Get(MediaInfoLib::Stream_Video);
	for (int videoStreamId = 0; videoStreamId < _videoStreamCount; videoStreamId++) {
		insertVideoStream(videoStreamId, VideoBitrate, QString::number(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("BitRate"))).trimmed().toInt() / 1000));
		insertVideoStream(videoStreamId, VideoWidth, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Width"))).trimmed());
		insertVideoStream(videoStreamId, VideoHeight, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Height"))).trimmed());
		insertVideoStream(videoStreamId, VideoFrameRate, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("FrameRate"))).trimmed());
		insertVideoStream(videoStreamId, VideoFormat, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Format"))).trimmed());
		insertVideoStream(videoStreamId, VideoCodec, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Codec/String"))).trimmed());
		insertVideoStream(videoStreamId, VideoEncodedLibrary, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Encoded_Library/String"))).trimmed());
	}

	//Text
	_textStreamCount = mediaInfo.Count_Get(MediaInfoLib::Stream_Text);
	for (int textStreamId = 0; textStreamId < _textStreamCount; textStreamId++) {
		insertTextStream(textStreamId, TextFormat, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Text, textStreamId, _T("Format"))).trimmed());
		insertTextStream(textStreamId, TextLanguage, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Text, textStreamId, _T("Language/String"))).trimmed());
	}

	_fetched = true;
	emit fetched();
#endif	//MEDIAINFOLIB
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

QString MediaInfoFetcher::fileSize() const {
	if (_fileSize > 0) {
		return QString::number(_fileSize / 1024 / 1024.0);
	} else {
		return QString();
	}
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

QString MediaInfoFetcher::encodedApplication() const {
	return _encodedApplication;
}

//Metadata
QString MediaInfoFetcher::metadataValue(Metadata metadata) const {
	return _metadataHash.value(metadata);
}

void MediaInfoFetcher::insertMetadata(Metadata metadata, const QString & value) {
	if (!_metadataHash.contains(metadata)) {
		_metadataHash.insert(metadata, value);
	} else {
		qCritical() << __FUNCTION__ << "Error: key already inserted:" << metadata;
	}
}

//Audio
int MediaInfoFetcher::audioStreamCount() const {
	return _audioStreamCount;
}

QString MediaInfoFetcher::audioStreamValue(int audioStreamId, AudioStream audioStream) const {
	//Constructs the key
	//Example: audioStreamId = 1, audioStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(audioStreamId) + QString::number(audioStream)).toInt();

	return _audioStreamHash.value(key);
}

void MediaInfoFetcher::insertAudioStream(int audioStreamId, AudioStream audioStream, const QString & value) {
	//Constructs the key
	//Example: audioStreamId = 1, audioStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(audioStreamId) + QString::number(audioStream)).toInt();

	if (!_audioStreamHash.contains(key)) {
		_audioStreamHash.insert(key, value);
	} else {
		qCritical() << __FUNCTION__ << "Error: key already inserted:" << key;
	}
}

//Video
int MediaInfoFetcher::videoStreamCount() const {
	return _videoStreamCount;
}

QString MediaInfoFetcher::videoStreamValue(int videoStreamId, VideoStream videoStream) const {
	//Constructs the key
	//Example: videoStreamId = 1, videoStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(videoStreamId) + QString::number(videoStream)).toInt();

	return _videoStreamHash.value(key);
}

void MediaInfoFetcher::insertVideoStream(int videoStreamId, VideoStream videoStream, const QString & value) {
	//Constructs the key
	//Example: videoStreamId = 1, videoStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(videoStreamId) + QString::number(videoStream)).toInt();

	if (!_videoStreamHash.contains(key)) {
		_videoStreamHash.insert(key, value);
	} else {
		qCritical() << __FUNCTION__ << "Error: key already inserted:" << key;
	}
}

//Text
int MediaInfoFetcher::textStreamCount() const {
	return _textStreamCount;
}

QString MediaInfoFetcher::textStreamValue(int textStreamId, TextStream textStream) const {
	//Constructs the key
	//Example: textStreamId = 1, textStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(textStreamId) + QString::number(textStream)).toInt();

	return _textStreamHash.value(key);
}

void MediaInfoFetcher::insertTextStream(int textStreamId, TextStream textStream, const QString & value) {
	//Constructs the key
	//Example: textStreamId = 1, textStream = 9
	//key = 19, not 1 + 9 = 10!
	int key = QString(QString::number(textStreamId) + QString::number(textStream)).toInt();

	if (!_textStreamHash.contains(key)) {
		_textStreamHash.insert(key, value);
	} else {
		qCritical() << __FUNCTION__ << "Error: key already inserted:" << key;
	}
}

//Network stream
QString MediaInfoFetcher::networkStreamValue(NetworkStream networkStream) const {
	return _networkStreamHash.value(networkStream);
}

void MediaInfoFetcher::insertNetworkStream(NetworkStream networkStream, const QString & value) {
	if (!_networkStreamHash.contains(networkStream)) {
		_networkStreamHash.insert(networkStream, value);
	} else {
		qCritical() << __FUNCTION__ << "Error: key already inserted:" << networkStream;
	}
}
