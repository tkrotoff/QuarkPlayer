/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "MediaInfoFetcher.h"

#include "MediaInfoFetcherLogger.h"

#include <FileTypes/FileTypes.h>

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
	//MediaInfo is compiled with Unicode support on
	//UNICODE symbol already defined for all of QuarkPlayer
	//#define UNICODE
	#ifdef Q_WS_WIN
		#include <MediaInfo/MediaInfoDLL.h>
		#define MediaInfoLib MediaInfoDLL
	#else
		#include <MediaInfo/MediaInfo.h>
	#endif	//Q_WS_WIN
#endif	//MEDIAINFOLIB

#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QFile>
#include <QtCore/QSize>
#include <QtCore/QtConcurrentRun>

//FIXME
//QFuture returned by QtConcurrent::run() does not support canceling, pausing, or progress reporting
//That is why we need to know when MediaInfoFetcher gets destroyed
//and cancel any running QtConcurrent::run()
//There is something wrong here: startMediaInfoLibResolver() and startTagLibResolver()
//should be independant functions with a hard copy of the parameter MediaInfo
//otherwise it will always crash inside this->_mediaInfo
static bool _destroyed = false;

MediaInfoFetcher::MediaInfoFetcher(QObject * parent)
	: QObject(parent) {

	_metaObjectInfoResolver = NULL;

	_mediaInfo.clear();
}

MediaInfoFetcher::~MediaInfoFetcher() {
	_destroyed = true;
}

MediaInfo MediaInfoFetcher::mediaInfo() const {
	return _mediaInfo;
}

void MediaInfoFetcher::emitFinishedSignal() {
	_mediaInfo.setFetched(true);
	emit finished(_mediaInfo);
}

void MediaInfoFetcher::start(const MediaInfo & mediaInfo, ReadStyle readStyle) {
	//By using an already existing MediaInfo as a parameter, if informations are found inside
	//a .m3u playlist about this track, these informations won't be replaced by empty informations
	_mediaInfo = mediaInfo;

	_readStyle = readStyle;

	QString fileName(_mediaInfo.fileName());
	bool isUrl = MediaInfo::isUrl(fileName);
	if (isUrl) {
		//Cannot solve meta data from a stream/remote media
		//This might be caused also by an invalid filename?
		MediaInfoFetcherCritical() << "mediaInfo is a URL";

		emitFinishedSignal();
	} else {
		determineFileTypeFromExtension();

		bool resolverLaunched = false;

#ifdef MEDIAINFOLIB
		if (_readStyle == ReadStyleAccurate) {
			MediaInfoFetcherDebug() << "MediaInfoLib";
			QtConcurrent::run(this, &MediaInfoFetcher::startMediaInfoLibResolver);
			resolverLaunched = true;
		}
#endif	//MEDIAINFOLIB

#ifdef TAGLIB
		if (!resolverLaunched) {
			MediaInfoFetcherDebug() << "TagLib";

			//Use TagLib only for files on the harddrive, not for URLs
			//See http://article.gmane.org/gmane.comp.kde.devel.taglib/864
			//Run it inside a different thread since TagLib is a synchronous library
			QtConcurrent::run(this, &MediaInfoFetcher::startTagLibResolver);
			resolverLaunched = true;
		}
#endif	//TAGLIB

		if (!resolverLaunched) {
			MediaInfoFetcherDebug() << "Phonon";

			//If TagLib or MediaInfoLib are not used, let's use
			//the backend for resolving the metaData
			_mediaSource = fileName;
			startPhononResolver();
			resolverLaunched = true;
		}
	}
}

void MediaInfoFetcher::start(Phonon::MediaObject * mediaObject) {
	_mediaInfo.clear();

	_mediaSource = mediaObject->currentSource();

	bool isUrl = (_mediaSource.type() == Phonon::MediaSource::Url);
	if (isUrl) {
		_mediaInfo.setFileName(_mediaSource.url().toString());
	} else {
		_mediaInfo.setFileName(_mediaSource.fileName());
		MediaInfoFetcherCritical() << "mediaSource is not a URL";
	}

	//Cannot solve metaData from a stream/remote media if we have only the MediaSource
	//We need the MediaObject
	//Use the given mediaObject to get the meta data
	//So when using MediaInfoFetcher you must check if the source is a url
	//and in this case provide a mediaObject
	//Couldn't find a better solution :/

	//Save _metaObjectInfoResolver
	Phonon::MediaObject * savedMetaObjectInfoResolver = _metaObjectInfoResolver;

	//Use the given mediaObject
	_metaObjectInfoResolver = mediaObject;

	//Trick: the given mediaObject is already playing thus we have already
	//all the metaData from the stream, we just need to read them
	metaStateChanged(Phonon::StoppedState, Phonon::StoppedState);

	//Back to normal
	_metaObjectInfoResolver = savedMetaObjectInfoResolver;
}

void MediaInfoFetcher::startPhononResolver() {
	if (_destroyed) {
		MediaInfoFetcherCritical() << "Kill QtConcurrent::run()";
	}

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

	QMap<QString, QString> metaData = _metaObjectInfoResolver->metaData();

	if (newState == Phonon::ErrorState) {
		emitFinishedSignal();
	}

	if (newState == Phonon::StoppedState) {
		_mediaInfo.setMetaData(MediaInfo::TrackNumber, metaData.value("TRACKNUMBER").trimmed().toInt());
		_mediaInfo.setMetaData(MediaInfo::Title, metaData.value("TITLE").trimmed());
		_mediaInfo.setMetaData(MediaInfo::Artist, metaData.value("ARTIST").trimmed());
		_mediaInfo.setMetaData(MediaInfo::Album, metaData.value("ALBUM").trimmed());
		_mediaInfo.setMetaData(MediaInfo::Year, QDate::fromString(metaData.value("DATE").trimmed()));
		_mediaInfo.setMetaData(MediaInfo::Genre, metaData.value("GENRE").trimmed());
		_mediaInfo.setMetaData(MediaInfo::Comment, metaData.value("COMMENT").trimmed());

		_mediaInfo.setDurationMSecs(metaData.value("LENGTH").trimmed().toULongLong());
		//Converts from bps to kbps
		_mediaInfo.insertAudioStream(0, MediaInfo::AudioBitrate, metaData.value("BITRATE").trimmed().toInt() / 1000);
		_mediaInfo.setBitrate(metaData.value("BITRATE").trimmed().toInt() / 1000);
		_mediaInfo.insertAudioStream(0, MediaInfo::AudioSampleRate, metaData.value("SAMPLERATE").trimmed().toInt());
		_mediaInfo.insertAudioStream(0, MediaInfo::AudioChannelCount, metaData.value("CHANNELS").trimmed().toInt());

		_mediaInfo.insertNetworkStream(MediaInfo::StreamName, metaData.value("STREAM_NAME").trimmed());
		_mediaInfo.insertNetworkStream(MediaInfo::StreamGenre, metaData.value("STREAM_GENRE").trimmed());
		_mediaInfo.insertNetworkStream(MediaInfo::StreamWebsite, QUrl(metaData.value("STREAM_WEBSITE").trimmed()));
		_mediaInfo.insertNetworkStream(MediaInfo::StreamURL, QUrl(metaData.value("STREAM_URL").trimmed()));

		emitFinishedSignal();
	}
}

#ifdef TAGLIB
QString parseID3v2_TXXX(const TagLib::ID3v2::FrameListMap & metaData, const QString & tagName) {
	QString tmp;
	TagLib::ID3v2::FrameList frameList = metaData["TXXX"];
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

QString parseID3v2_WXXX(const TagLib::ID3v2::FrameListMap & metaData) {
	QString tmp;
	TagLib::ID3v2::FrameList frameList = metaData["WXXX"];
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
	if (_destroyed) {
		MediaInfoFetcherCritical() << "Kill QtConcurrent::run()";
	}

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
		MediaInfoFetcherCritical() << "Unknown ReadStyle:" << _readStyle;
	}

	//Taken from Amarok, file: CollectionScanner.cpp
	//See http://gitorious.org/amarok/amarok/blobs/master/utilities/collectionscanner/CollectionScanner.cpp
#ifdef Q_WS_WIN
	const wchar_t * encodedName = reinterpret_cast<const wchar_t *>(_mediaInfo.fileName().utf16());
#else
	QByteArray fileName = QFile::encodeName(_mediaInfo.fileName());
	const char * encodedName = fileName.constData(); //Valid as long as fileName exists
#endif	//Q_WS_WIN

	TagLib::FileRef fileRef(encodedName, true, readStyle);

	if (fileRef.isNull()) {
		MediaInfoFetcherCritical() << "fileRef is null:" << _mediaInfo.fileName();
	} else {

		TagLib::Tag * tag = fileRef.tag();
		if (tag) {
			//Do it before anything else, very generic, can contains ID3v1 tags I guess
			_mediaInfo.setMetaData(MediaInfo::TrackNumber, QString::number(tag->track()));
			_mediaInfo.setMetaData(MediaInfo::Title, TStringToQString(tag->title()).trimmed());
			_mediaInfo.setMetaData(MediaInfo::Artist, TStringToQString(tag->artist()).trimmed());
			_mediaInfo.setMetaData(MediaInfo::Album, TStringToQString(tag->album()).trimmed());
			_mediaInfo.setMetaData(MediaInfo::Year, QString::number(tag->year()));
			_mediaInfo.setMetaData(MediaInfo::Genre, TStringToQString(tag->genre()).trimmed());
			_mediaInfo.setMetaData(MediaInfo::Comment, TStringToQString(tag->comment()).trimmed());
		}

		if (TagLib::MPEG::File * file = dynamic_cast<TagLib::MPEG::File *>(fileRef.file())) {
			_mediaInfo.setFileType(FileTypes::fileType(FileType::MP3));
			if (file->ID3v2Tag()) {
				TagLib::ID3v2::FrameListMap metaData = file->ID3v2Tag()->frameListMap();

				//4 letters: id3v2.3.0 and id3v2.4.0 (November 1, 2000)
				//3 letters: id3v2.2.0, "considered obsolete" cf http://en.wikipedia.org/wiki/ID3

				if (!metaData["TPOS"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::DiscNumber, TStringToQString(metaData["TPOS"].front()->toString()).trimmed());
				}
				if (!metaData["TBP"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::BPM, TStringToQString(metaData["TBP"].front()->toString()).trimmed().toInt());
				}
				if (!metaData["TBPM"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::BPM, TStringToQString(metaData["TBPM"].front()->toString()).trimmed().toInt());
				}
				if (!metaData["TCM"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::Composer, TStringToQString(metaData["TCM"].front()->toString()).trimmed());
				}
				if (!metaData["TCOM"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::Composer, TStringToQString(metaData["TCOM"].front()->toString()).trimmed());
				}
				if (!metaData["TPE2"].isEmpty()) {
					//Non-standard: Apple, Microsoft
					_mediaInfo.setMetaData(MediaInfo::AlbumArtist, TStringToQString(metaData["TPE2"].front()->toString()).trimmed());
				}
				if (!metaData["TCMP"].isEmpty()) {
					//TODO
					MediaInfoFetcherDebug() << "TODO Compilation:" << TStringToQString(metaData["TCMP"].front()->toString()).trimmed();
				}
				if (!metaData["TPB"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::Publisher, TStringToQString(metaData["TPB"].front()->toString()).trimmed());
				}
				if (!metaData["TPUB"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::Publisher, TStringToQString(metaData["TPUB"].front()->toString()).trimmed());
				}
				if (!metaData["TCR"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::Copyright, TStringToQString(metaData["TCR"].front()->toString()).trimmed());
				}
				if (!metaData["TCOP"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::Copyright, TStringToQString(metaData["TCOP"].front()->toString()).trimmed());
				}
				if (!metaData["TEN"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::EncodedBy, TStringToQString(metaData["TEN"].front()->toString()).trimmed());
				}
				if (!metaData["TENC"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::EncodedBy, TStringToQString(metaData["TENC"].front()->toString()).trimmed());
				}
				if (!metaData["TOA"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::OriginalArtist, TStringToQString(metaData["TOA"].front()->toString()).trimmed());
				}
				if (!metaData["TOPE"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::OriginalArtist, TStringToQString(metaData["TOPE"].front()->toString()).trimmed());
				}

				_mediaInfo.setMetaData(MediaInfo::URL, QUrl(parseID3v2_WXXX(metaData)));

				_mediaInfo.setMetaData(MediaInfo::AlbumArtistSort, parseID3v2_TXXX(metaData, "ALBUMARTISTSORT"));

				_mediaInfo.setMetaData(MediaInfo::MusicBrainzArtistId, /*QUuid(*/parseID3v2_TXXX(metaData, "MusicBrainz Artist Id")/*)*/);
				_mediaInfo.setMetaData(MediaInfo::MusicBrainzReleaseId, /*QUuid(*/parseID3v2_TXXX(metaData, "MusicBrainz Album Id")/*)*/);
				_mediaInfo.setMetaData(MediaInfo::MusicBrainzTrackId, /*QUuid(*/parseID3v2_TXXX(metaData, "MusicBrainz Track Id")/*)*/);
				_mediaInfo.setMetaData(MediaInfo::AmazonASIN, parseID3v2_TXXX(metaData, "ASIN"));
			}

		} else if (TagLib::Ogg::Vorbis::File * file = dynamic_cast<TagLib::Ogg::Vorbis::File *>(fileRef.file())) {
			_mediaInfo.setFileType(FileTypes::fileType(FileType::Vorbis));
			if (file->tag()) {
				TagLib::Ogg::FieldListMap metaData = file->tag()->fieldListMap();
				if (!metaData["COMPOSER"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::Composer, TStringToQString(metaData["COMPOSER"].front()).trimmed());
				}
				if (!metaData["BPM"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::BPM, TStringToQString(metaData["BPM"].front()).trimmed().toInt());
				}
				if (!metaData["DISCNUMBER"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::DiscNumber, TStringToQString(metaData["DISCNUMBER"].front()).trimmed().toInt());
				}
				if (!metaData["COMPILATION"].isEmpty()) {
					//TODO
					MediaInfoFetcherDebug() << "TODO Compilation:" << TStringToQString(metaData["COMPILATION"].front()).trimmed();
				}
			}

		} else if (TagLib::Ogg::FLAC::File * file = dynamic_cast<TagLib::Ogg::FLAC::File *>(fileRef.file())) {
			_mediaInfo.setFileType(FileTypes::fileType(FileType::FLAC));
			if (file->tag()) {
				TagLib::Ogg::FieldListMap metaData = file->tag()->fieldListMap();
				if (!metaData["COMPOSER"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::Composer, TStringToQString(metaData["COMPOSER"].front()).trimmed());
				}
				if (!metaData["BPM"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::BPM, TStringToQString(metaData["BPM"].front()).trimmed().toInt());
				}
				if (!metaData["DISCNUMBER"].isEmpty()) {
					_mediaInfo.setMetaData(MediaInfo::DiscNumber, TStringToQString(metaData["DISCNUMBER"].front()).trimmed().toInt());
				}
				if (!metaData["COMPILATION"].isEmpty()) {
					//TODO
					MediaInfoFetcherDebug() << "TODO Compilation:" << TStringToQString(metaData["COMPILATION"].front()).trimmed();
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

		TagLib::AudioProperties * audioProperties = fileRef.audioProperties();
		if (audioProperties) {
			_mediaInfo.setDurationSecs(audioProperties->length());
			_mediaInfo.insertAudioStream(0, MediaInfo::AudioBitrate, audioProperties->bitrate());
			_mediaInfo.setBitrate(audioProperties->bitrate());
			_mediaInfo.insertAudioStream(0, MediaInfo::AudioSampleRate, audioProperties->sampleRate());
			_mediaInfo.insertAudioStream(0, MediaInfo::AudioChannelCount, audioProperties->channels());
		}
	}

	emitFinishedSignal();
#endif	//TAGLIB
}

void MediaInfoFetcher::startMediaInfoLibResolver() {
	if (_destroyed) {
		MediaInfoFetcherCritical() << "Kill QtConcurrent::run()";
	}

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

	//TODO check for errors
	mediaInfo.Open(_mediaInfo.fileName().toStdWString());

	//Info_Parameters: gets all useful MediaInfoLib parameters names
	//mediaInfo.Option(_T("Info_Parameters"));
	//mediaInfo.Option(_T("Complete"), _T("1"));
	//MediaInfoFetcherDebug() << QString::fromStdWString(mediaInfo.Inform());

	//General
	_mediaInfo.setFileSize(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("FileSize"))).toInt());
	_mediaInfo.setDurationMSecs(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Duration"))).toULongLong());
	_mediaInfo.setBitrate(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("OverallBitRate"))).toInt() / 1000);
	_mediaInfo.setEncodedApplication(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Encoded_Application"))));

	//Metadata
	_mediaInfo.setMetaData(MediaInfo::Title, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Title"))).trimmed());
	_mediaInfo.setMetaData(MediaInfo::Artist, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Performer"))).trimmed());
	_mediaInfo.setMetaData(MediaInfo::Album, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Album"))).trimmed());
	_mediaInfo.setMetaData(MediaInfo::AlbumArtist, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Accompaniment"))).trimmed());
	_mediaInfo.setMetaData(MediaInfo::TrackNumber, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Track/Position"))).trimmed().toInt());
	_mediaInfo.setMetaData(MediaInfo::DiscNumber, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Part/Position"))).trimmed().toInt());
	_mediaInfo.setMetaData(MediaInfo::OriginalArtist, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Original/Performer"))).trimmed());
	_mediaInfo.setMetaData(MediaInfo::Composer, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Composer"))).trimmed());
	_mediaInfo.setMetaData(MediaInfo::Publisher, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Publisher"))).trimmed());
	_mediaInfo.setMetaData(MediaInfo::Genre, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Genre"))).trimmed());
	_mediaInfo.setMetaData(MediaInfo::Year, QDate::fromString(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Recorded_Date"))).trimmed()));
	_mediaInfo.setMetaData(MediaInfo::BPM, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("BPM"))).trimmed());
	_mediaInfo.setMetaData(MediaInfo::Copyright, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Copyright"))).trimmed());
	_mediaInfo.setMetaData(MediaInfo::Comment, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Comment"))).trimmed());
	_mediaInfo.setMetaData(MediaInfo::URL, QUrl(QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("URL"))).trimmed()));
	_mediaInfo.setMetaData(MediaInfo::EncodedBy, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("Encoded_Library/String"))).trimmed());
	_mediaInfo.setMetaData(MediaInfo::MusicBrainzArtistId, /*QUuid(*/QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("MusicBrainz Artist Id"))).trimmed()/*)*/);
	_mediaInfo.setMetaData(MediaInfo::MusicBrainzReleaseId, /*QUuid(*/QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("MusicBrainz Album Id"))).trimmed()/*)*/);
	_mediaInfo.setMetaData(MediaInfo::MusicBrainzTrackId, /*QUuid(*/QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("MusicBrainz Track Id"))).trimmed()/*)*/);
	_mediaInfo.setMetaData(MediaInfo::AmazonASIN, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_General, 0, _T("ASIN"))).trimmed());

	//Audio
	int audioStreamCount = mediaInfo.Count_Get(MediaInfoLib::Stream_Audio);
	for (int audioStreamId = 0; audioStreamId < audioStreamCount; audioStreamId++) {
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioBitrate, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("BitRate"))).trimmed().toInt() / 1000);
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioBitrateMode, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("BitRate_Mode"))).trimmed());
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioSampleRate, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("SamplingRate"))).trimmed().toFloat() / 1000.0);
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioBitsPerSample, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Resolution"))).trimmed().toUInt());
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioChannelCount, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Channel(s)"))).trimmed().toUInt());
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioCodec, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Codec/String"))).trimmed());
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioCodecProfile, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Codec_Profile"))).trimmed());
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioLanguage, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Language/String"))).trimmed());
		_mediaInfo.insertAudioStream(audioStreamId, MediaInfo::AudioEncodedLibrary, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Audio, audioStreamId, _T("Encoded_Library/String"))).trimmed());
	}

	//Video
	int videoStreamCount = mediaInfo.Count_Get(MediaInfoLib::Stream_Video);
	for (int videoStreamId = 0; videoStreamId < videoStreamCount; videoStreamId++) {
		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoBitrate, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("BitRate"))).trimmed().toUInt() / 1000);

		int width = QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Width"))).trimmed().toInt();
		int height = QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Height"))).trimmed().toInt();
		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoResolution, QSize(width, height));

		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoFrameRate, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("FrameRate"))).trimmed().toUInt());
		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoFormat, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Format"))).trimmed());
		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoCodec, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Codec/String"))).trimmed());
		_mediaInfo.insertVideoStream(videoStreamId, MediaInfo::VideoEncodedLibrary, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Video, videoStreamId, _T("Encoded_Library/String"))).trimmed());
	}

	//Text
	int textStreamCount = mediaInfo.Count_Get(MediaInfoLib::Stream_Text);
	for (int textStreamId = 0; textStreamId < textStreamCount; textStreamId++) {
		_mediaInfo.insertTextStream(textStreamId, MediaInfo::TextFormat, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Text, textStreamId, _T("Format"))).trimmed());
		_mediaInfo.insertTextStream(textStreamId, MediaInfo::TextLanguage, QString::fromStdWString(mediaInfo.Get(MediaInfoLib::Stream_Text, textStreamId, _T("Language/String"))).trimmed());
	}

	emitFinishedSignal();
#endif	//MEDIAINFOLIB
}

void MediaInfoFetcher::determineFileTypeFromExtension() {
	QString extension(QFileInfo(_mediaInfo.fileName()).suffix());
	_mediaInfo.setFileType(FileTypes::fileType(extension));
}
