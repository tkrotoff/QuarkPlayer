/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtCore/QFileInfo>

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
	//MediaInfo is compiled with Unicode support on
	#define UNICODE
	#ifdef Q_OS_UNIX
		#include <MediaInfo/MediaInfo.h>
	#else
		#include <MediaInfo/MediaInfoDLL.h>
		#define MediaInfoLib MediaInfoDLL
	#endif	//Q_OS_UNIX
#endif	//MEDIAINFOLIB

#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QDebug>

MediaInfoFetcher::MediaInfoFetcher(QObject * parent)
	: QObject(parent) {

	_metaObjectInfoResolver = NULL;

	_mediaInfo.clear();
}

MediaInfoFetcher::~MediaInfoFetcher() {
}

MediaInfo MediaInfoFetcher::mediaInfo() const {
	return _mediaInfo;
}

void MediaInfoFetcher::start(const Phonon::MediaSource & mediaSource, ReadStyle readStyle) {
	_mediaInfo.clear();

	_mediaSource = mediaSource;
	_readStyle = readStyle;

	if (_mediaSource.type() == Phonon::MediaSource::Url) {
		_mediaInfo.setFileName(_mediaSource.url().toString());
		_mediaInfo.setUrl(true);

		//Cannot solve meta data from a stream/remote media
		qCritical() << __FUNCTION__ << "Error: mediaSource is a URL";

		//This might be caused also by an invalid filename

		//Sends the fetched() signal
		_mediaInfo.setFetched(true);
		emit fetched();
	} else {
		_mediaInfo.setFileName(_mediaSource.fileName());
		_mediaInfo.setUrl(false);

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
	_mediaInfo.clear();

	_mediaSource = mediaObject->currentSource();

	if (_mediaSource.type() == Phonon::MediaSource::Url) {
		_mediaInfo.setFileName(_mediaSource.url().toString());
		_mediaInfo.setUrl(true);
	} else {
		_mediaInfo.setFileName(_mediaSource.fileName());
		_mediaInfo.setUrl(false);
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
	Q_UNUSED(oldState);

	QMap<QString, QString> metadata = _metaObjectInfoResolver->metaData();

	if (newState == Phonon::ErrorState) {
		_mediaInfo.setFetched(true);
		emit fetched();
	}

	if (newState == Phonon::StoppedState) {
		_mediaInfo.insertMetadata(MediaInfo::TrackNumber, metadata.value("TRACKNUMBER").trimmed());
		_mediaInfo.insertMetadata(MediaInfo::Title, metadata.value("TITLE").trimmed());
		_mediaInfo.insertMetadata(MediaInfo::Artist, metadata.value("ARTIST").trimmed());
		_mediaInfo.insertMetadata(MediaInfo::Album, metadata.value("ALBUM").trimmed());
		_mediaInfo.insertMetadata(MediaInfo::Year, metadata.value("DATE").trimmed());
		_mediaInfo.insertMetadata(MediaInfo::Genre, metadata.value("GENRE").trimmed());
		_mediaInfo.insertMetadata(MediaInfo::Comment, metadata.value("COMMENT").trimmed());

		//Converts from milliseconds to seconds
		_mediaInfo.setLength(static_cast<int>(metadata.value("LENGTH").toInt() / 1000.0));
		//Converts from bps to kbps
		_mediaInfo.insertAudioStream(0, MediaInfo::AudioBitrate, QString::number(metadata.value("BITRATE").trimmed().toInt() / 1000));
		_mediaInfo.insertAudioStream(0, MediaInfo::AudioSampleRate, metadata.value("SAMPLERATE").trimmed());
		_mediaInfo.insertAudioStream(0, MediaInfo::AudioChannelCount, metadata.value("CHANNELS").trimmed());

		_mediaInfo.insertNetworkStream(MediaInfo::StreamName, metadata.value("STREAM_NAME").trimmed());
		_mediaInfo.insertNetworkStream(MediaInfo::StreamGenre, metadata.value("STREAM_GENRE").trimmed());
		_mediaInfo.insertNetworkStream(MediaInfo::StreamWebsite, metadata.value("STREAM_WEBSITE").trimmed());
		_mediaInfo.insertNetworkStream(MediaInfo::StreamURL, metadata.value("STREAM_URL").trimmed());

		_mediaInfo.setFetched(true);
		emit fetched();
	}
}

#ifdef TAGLIB
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
#endif	//TAGLIB

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
	const wchar_t * encodedName = reinterpret_cast<const wchar_t *>(_mediaInfo.fileName().utf16());
#else
	const char * encodedName = QFile::encodeName(_mediaInfo.fileName()).constData();
#endif	//Q_OS_WIN

	TagLib::FileRef fileRef(encodedName, true, readStyle);

	if (fileRef.isNull()) {
		qCritical() << __FUNCTION__ << "Error: the FileRef is null:" << _mediaInfo.fileName();
	} else {

		if (TagLib::MPEG::File * file = dynamic_cast<TagLib::MPEG::File *>(fileRef.file())) {
			_mediaInfo.setFileType(FileTypes::fileType(FileType::MP3));
			if (file->ID3v2Tag()) {
				TagLib::ID3v2::FrameListMap metadata = file->ID3v2Tag()->frameListMap();

				//4 letters: id3v2.3.0 and id3v2.4.0 (November 1, 2000)
				//3 letters: id3v2.2.0, "considered obsolete" cf http://en.wikipedia.org/wiki/ID3

				if (!metadata["TPOS"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::DiscNumber, TStringToQString(metadata["TPOS"].front()->toString()).trimmed());
				}
				if (!metadata["TBP"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::BPM, TStringToQString(metadata["TBP"].front()->toString()).trimmed());
				}
				if (!metadata["TBPM"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::BPM, TStringToQString(metadata["TBPM"].front()->toString()).trimmed());
				}
				if (!metadata["TCM"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::Composer, TStringToQString(metadata["TCM"].front()->toString()).trimmed());
				}
				if (!metadata["TCOM"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::Composer, TStringToQString(metadata["TCOM"].front()->toString()).trimmed());
				}
				if (!metadata["TPE2"].isEmpty()) {
					//Non-standard: Apple, Microsoft
					_mediaInfo.insertMetadata(MediaInfo::AlbumArtist, TStringToQString(metadata["TPE2"].front()->toString()).trimmed());
				}
				if (!metadata["TCMP"].isEmpty()) {
					//TODO
					qDebug() << "Compilation:" << TStringToQString(metadata["TCMP"].front()->toString()).trimmed();
				}
				if (!metadata["TPB"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::Publisher, TStringToQString(metadata["TPB"].front()->toString()).trimmed());
				}
				if (!metadata["TPUB"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::Publisher, TStringToQString(metadata["TPUB"].front()->toString()).trimmed());
				}
				if (!metadata["TCR"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::Copyright, TStringToQString(metadata["TCR"].front()->toString()).trimmed());
				}
				if (!metadata["TCOP"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::Copyright, TStringToQString(metadata["TCOP"].front()->toString()).trimmed());
				}
				if (!metadata["TEN"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::EncodedBy, TStringToQString(metadata["TEN"].front()->toString()).trimmed());
				}
				if (!metadata["TENC"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::EncodedBy, TStringToQString(metadata["TENC"].front()->toString()).trimmed());
				}
				if (!metadata["TOA"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::OriginalArtist, TStringToQString(metadata["TOA"].front()->toString()).trimmed());
				}
				if (!metadata["TOPE"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::OriginalArtist, TStringToQString(metadata["TOPE"].front()->toString()).trimmed());
				}

				_mediaInfo.insertMetadata(MediaInfo::URL, parseID3v2_WXXX(metadata));

				_mediaInfo.insertMetadata(MediaInfo::AlbumArtistSort, parseID3v2_TXXX(metadata, "ALBUMARTISTSORT"));

				_mediaInfo.insertMetadata(MediaInfo::MusicBrainzArtistId, parseID3v2_TXXX(metadata, "MusicBrainz Artist Id"));
				_mediaInfo.insertMetadata(MediaInfo::MusicBrainzReleaseId, parseID3v2_TXXX(metadata, "MusicBrainz Album Id"));
				_mediaInfo.insertMetadata(MediaInfo::MusicBrainzTrackId, parseID3v2_TXXX(metadata, "MusicBrainz Track Id"));
				_mediaInfo.insertMetadata(MediaInfo::AmazonASIN, parseID3v2_TXXX(metadata, "ASIN"));
			}

		} else if (TagLib::Ogg::Vorbis::File * file = dynamic_cast<TagLib::Ogg::Vorbis::File *>(fileRef.file())) {
			_mediaInfo.setFileType(FileTypes::fileType(FileType::Vorbis));
			if (file->tag()) {
				TagLib::Ogg::FieldListMap metadata = file->tag()->fieldListMap();
				if (!metadata["COMPOSER"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::Composer, TStringToQString(metadata["COMPOSER"].front()).trimmed());
				}
				if (!metadata["BPM"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::BPM, TStringToQString(metadata["BPM"].front()).trimmed());
				}
				if (!metadata["DISCNUMBER"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::DiscNumber, TStringToQString(metadata["DISCNUMBER"].front()).trimmed());
				}
				if (!metadata["COMPILATION"].isEmpty()) {
					//TODO
					qDebug() << "Compilation:" << TStringToQString(metadata["COMPILATION"].front()).trimmed();
				}
			}

		} else if (TagLib::Ogg::FLAC::File * file = dynamic_cast<TagLib::Ogg::FLAC::File *>(fileRef.file())) {
			_mediaInfo.setFileType(FileTypes::fileType(FileType::FLAC));
			if (file->tag()) {
				TagLib::Ogg::FieldListMap metadata = file->tag()->fieldListMap();
				if (!metadata["COMPOSER"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::Composer, TStringToQString(metadata["COMPOSER"].front()).trimmed());
				}
				if (!metadata["BPM"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::BPM, TStringToQString(metadata["BPM"].front()).trimmed());
				}
				if (!metadata["DISCNUMBER"].isEmpty()) {
					_mediaInfo.insertMetadata(MediaInfo::DiscNumber, TStringToQString(metadata["DISCNUMBER"].front()).trimmed());
				}
				if (!metadata["COMPILATION"].isEmpty()) {
					//TODO
					qDebug() << "Compilation:" << TStringToQString(metadata["COMPILATION"].front()).trimmed();
				}
			}

		} else if (dynamic_cast<TagLib::Ogg::Speex::File *>(fileRef.file())) {
			_mediaInfo.setFileType(FileTypes::fileType(FileType::Speex));
		} else if (dynamic_cast<TagLib::MPC::File *>(fileRef.file())) {
			_mediaInfo.setFileType(FileTypes::fileType(FileType::MPC));
		} else if (dynamic_cast<TagLib::WavPack::File *>(fileRef.file())) {
			_mediaInfo.setFileType(FileTypes::fileType(FileType::WavPack));
		} else if (dynamic_cast<TagLib::TrueAudio::File *>(fileRef.file())) {
			_mediaInfo.setFileType(FileTypes::fileType(FileType::TTA));
		} else if (dynamic_cast<TagLib::ASF::File *>(fileRef.file())) {
			//ASF is a container, WMA is the real codec, see http://en.wikipedia.org/wiki/Advanced_Systems_Format
			//and http://en.wikipedia.org/wiki/Windows_Media_Audio
			//Let's ASF be WMA, more consistent for the users
			_mediaInfo.setFileType(FileTypes::fileType(FileType::WMA));
		} else if (dynamic_cast<TagLib::MP4::File *>(fileRef.file())) {
			//MP4 is in fact AAC (.m4a), see http://en.wikipedia.org/wiki/Advanced_Audio_Coding
			//and http://en.wikipedia.org/wiki/MPEG-4_Part_14
			_mediaInfo.setFileType(FileTypes::fileType(FileType::AAC));
		}


		_mediaInfo.setFileSize(fileRef.file()->length());
		TagLib::Tag * tag = fileRef.tag();
		if (tag) {
			if (!tag->isEmpty()) {
				_mediaInfo.insertMetadata(MediaInfo::TrackNumber, QString::number(tag->track()));
				_mediaInfo.insertMetadata(MediaInfo::Title, TStringToQString(tag->title()).trimmed());
				_mediaInfo.insertMetadata(MediaInfo::Artist, TStringToQString(tag->artist()).trimmed());
				_mediaInfo.insertMetadata(MediaInfo::Album, TStringToQString(tag->album()).trimmed());
				_mediaInfo.insertMetadata(MediaInfo::Year, QString::number(tag->year()));
				_mediaInfo.insertMetadata(MediaInfo::Genre, TStringToQString(tag->genre()).trimmed());
				_mediaInfo.insertMetadata(MediaInfo::Comment, TStringToQString(tag->comment()).trimmed());
			}
		}

		TagLib::AudioProperties * audioProperties = fileRef.audioProperties();
		if (audioProperties) {
			_mediaInfo.setLength(audioProperties->length());
			_mediaInfo.insertAudioStream(0, MediaInfo::AudioBitrate, QString::number(audioProperties->bitrate()));
			_mediaInfo.insertAudioStream(0, MediaInfo::AudioSampleRate, QString::number(audioProperties->sampleRate()));
			_mediaInfo.insertAudioStream(0, MediaInfo::AudioChannelCount, QString::number(audioProperties->channels()));
		}
	}

	_mediaInfo.setFetched(true);
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

	mediaInfo.Open(_mediaInfo.fileName().toStdWString());

	//Info_Parameters: gets all usefull MediaInfoLib parameters names
	//mediaInfo.Option(_T("Info_Parameters"));
	//mediaInfo.Option(_T("Complete"), _T("1"));
	//qDebug() << QString::fromStdWString(mediaInfo.Inform());

	//General
	_mediaInfo.setFileSize(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("FileSize"))).toInt());
	_mediaInfo.setLength(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Duration"))).toInt() / 1000);
	_mediaInfo.setBitrate(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("OverallBitRate"))).toInt() / 1000);
	_mediaInfo.setEncodedApplication(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Encoded_Application"))));

	//Metadata
	_mediaInfo.insertMetadata(MediaInfo::Title, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Title"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::Artist, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Performer"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::Album, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Album"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::AlbumArtist, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Accompaniment"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::TrackNumber, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Track/Position"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::DiscNumber, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Part/Position"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::OriginalArtist, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Original/Performer"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::Composer, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Composer"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::Publisher, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Publisher"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::Genre, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Genre"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::Year, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Recorded_Date"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::BPM, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("BPM"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::Copyright, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Copyright"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::Comment, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Comment"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::URL, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("URL"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::EncodedBy, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Encoded_Library/String"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::MusicBrainzArtistId, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("MusicBrainz Artist Id"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::MusicBrainzReleaseId, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("MusicBrainz Album Id"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::MusicBrainzTrackId, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("MusicBrainz Track Id"))).trimmed());
	_mediaInfo.insertMetadata(MediaInfo::AmazonASIN, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("ASIN"))).trimmed());

	//Audio
	size_t audioStreamCount = mediaInfo.Count_Get(MediaInfoLib::Stream_Audio);
	for (size_t audioStreamId = 0; audioStreamId < audioStreamCount; audioStreamId++) {
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioBitrate, QString::number(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("BitRate"))).trimmed().toInt() / 1000));
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioBitrateMode, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("BitRate_Mode"))).trimmed());
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioSampleRate, QString::number(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("SamplingRate"))).trimmed().toFloat() / 1000.0));
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioBitsPerSample, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Resolution"))).trimmed());
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioChannelCount, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Channel(s)"))).trimmed());
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioCodec, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Codec/String"))).trimmed());
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioCodecProfile, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Codec_Profile"))).trimmed());
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioLanguage, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Language/String"))).trimmed());
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioEncodedLibrary, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Encoded_Library/String"))).trimmed());
	}

	//Video
	size_t videoStreamCount = mediaInfo.Count_Get(MediaInfoLib::Stream_Video);
	for (size_t videoStreamId = 0; videoStreamId < videoStreamCount; videoStreamId++) {
		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoBitrate, QString::number(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("BitRate"))).trimmed().toInt() / 1000));
		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoWidth, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Width"))).trimmed());
		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoHeight, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Height"))).trimmed());
		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoFrameRate, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("FrameRate"))).trimmed());
		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoFormat, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Format"))).trimmed());
		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoCodec, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Codec/String"))).trimmed());
		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoEncodedLibrary, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Encoded_Library/String"))).trimmed());
	}

	//Text
	size_t textStreamCount = mediaInfo.Count_Get(MediaInfoLib::Stream_Text);
	for (size_t textStreamId = 0; textStreamId < textStreamCount; textStreamId++) {
		_mediaInfo.insertTextStream(textStreamId, MediaInfo::TextFormat, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Text, textStreamId, _T("Format"))).trimmed());
		_mediaInfo.insertTextStream(textStreamId, MediaInfo::TextLanguage, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Text, textStreamId, _T("Language/String"))).trimmed());
	}

	_mediaInfo.setFetched(true);
	emit fetched();
#endif	//MEDIAINFOLIB
}

void MediaInfoFetcher::determineFileTypeFromExtension() {
	QString extension(QFileInfo(_mediaInfo.fileName()).suffix());
	_mediaInfo.setFileType(FileTypes::fileType(extension));
}
