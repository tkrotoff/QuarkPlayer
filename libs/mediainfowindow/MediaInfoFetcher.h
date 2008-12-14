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

#ifndef MEDIAINFOFETCHER_H
#define MEDIAINFOFETCHER_H

#include <mediainfowindow/mediainfowindow_export.h>

#include <filetypes/FileType.h>

#include <phonon/phononnamespace.h>
#include <phonon/mediasource.h>

#include <QtCore/QString>

namespace Phonon {
	class MediaObject;
}

/**
 * Shows the album cover art and other infos about the media playing.
 *
 * @author Tanguy Krotoff
 */
class MEDIAINFOWINDOW_API MediaInfoFetcher : public QObject {
	Q_OBJECT
public:

	/**
	 * Constructs a MediaInfoFetcher.
	 *
	 * @param parent QObject parent
	 */
	MediaInfoFetcher(QObject * parent);

	~MediaInfoFetcher();

	enum ReadStyle {
		/** Read as little of the file as possible. */
		ReadStyleFast,

		/** Read as much of the file as needed to report accurate values. */
		ReadStyleAccurate
	};

	/**
	 * Starts info fetching given a media source.
	 *
	 * mediaSource cannot be a URL, only a file.
	 * If mediaSource is a URL then use start(Phonon::MediaObject * mediaObject)
	 *
	 * @param mediaSource Phonon media source
	 */
	void start(const Phonon::MediaSource & mediaSource, ReadStyle readStyle = ReadStyleFast);

	/**
	 * Starts info fetching given a media object.
	 *
	 * If mediaSource is not a URL, use start(const Phonon::MediaSource & mediaSource)
	 * If mediaSource is a URL then use this method.
	 *
	 * @param mediaObject media object, use MediaObject::currentMediaSource() internally
	 */
	void start(Phonon::MediaObject * mediaObject);

	/** Tells if the metadata were fetched or not. */
	bool hasBeenFetched() const;

	//General
	QString fileName() const;
	bool isUrl() const;
	FileType fileType() const;
	/** Returns the size of the file in kbytes. */
	QString fileSize() const;
	/** Returns the length of the file in seconds. */
	QString length() const;
	QString bitrate() const;
	QString encodedApplication() const;

	//Metadata
	enum Metadata {
		/** int */
		TrackNumber,
		/** int */
		TrackCount,
		/** QString */
		Title,
		/** QString */
		Artist,
		/** QString */
		OriginalArtist,
		/** QString */
		Album,
		/** QString */
		AlbumArtist,
		/** QString */
		Year,
		/** QString */
		Genre,
		/** QString */
		Comment,
		/** QString */
		Composer,
		/** QString */
		Publisher,
		/** QString */
		Copyright,
		/** QString */
		URL,
		/** QString */
		MusicBrainzArtistId,
		/** QString */
		MusicBrainzAlbumId,
		/** QString */
		MusicBrainzTrackId,
		//MusicBrainzDiscId
		/** int */
		BPM
	};

	QString metadataValue(Metadata metadata) const;

	//Audio
	enum AudioStream {
		/** int */
		AudioBitrate,
		/** QString */
		AudioBitrateMode,
		/** int */
		AudioSampleRate,
		/** int */
		AudioSampleBits,
		/** int */
		AudioChannelCount,
		/** QString */
		AudioCodec,
		/** QString */
		AudioCodecProfile,
		/** QString */
		AudioLanguage,
		/** QString */
		AudioEncodedLibrary
	};

	int audioStreamCount() const;

	QString audioStreamValue(int audioStreamId, AudioStream audioStream) const;

	//Video
	enum VideoStream {
		/** int */
		VideoBitrate,
		/** int */
		VideoWidth,
		/** int */
		VideoHeight,
		/** int */
		VideoFrameRate,
		/** QString */
		VideoFormat,
		/** QString */
		VideoCodec,
		/** QString */
		VideoEncodedLibrary
	};

	int videoStreamCount() const;

	QString videoStreamValue(int videoStreamId, VideoStream videoStream) const;

	//Text
	enum TextStream {
		/** QString */
		TextFormat,
		/** QString */
		TextLanguage
	};

	int textStreamCount() const;

	QString textStreamValue(int textStreamId, TextStream textStream) const;

	//Stream
	enum NetworkStream {
		/** QString */
		StreamName,
		/** QString */
		StreamGenre,
		/** QString */
		StreamWebsite,
		/** QString */
		StreamURL
	};

	QString networkStreamValue(NetworkStream networkStream) const;

signals:

	void fetched();

private slots:

	void metaStateChanged(Phonon::State newState, Phonon::State oldState);

private:

	void clear();

	/**
	 * Use Phonon to find metadata.
	 */
	void startPhononResolver();

	/**
	 * Use TagLib to find metadata.
	 *
	 * Call startTagLibResolver() only when you are sure that the backend start to play the file.
	 *
	 * TagLib open files in read/write, opening a file in read/write prevents the backend to open the file too :/
	 *
	 * @see VideoWidgetPlugin::stateChanged()
	 * @see http://article.gmane.org/gmane.comp.kde.devel.taglib/918
	 */
	void startTagLibResolver();

	/**
	 * Use MediaInfoLib to find metadata and other informations.
	 */
	void startMediaInfoLibResolver();

	/** Determines file type from the file extension. */
	void determineFileTypeFromExtension();

	ReadStyle _readStyle;

	Phonon::MediaSource _mediaSource;

	bool _fetched;

	/** Resolves media metadata/info. */
	Phonon::MediaObject * _metaObjectInfoResolver;

	//General
	QString _filename;
	bool _isUrl;
	FileType _fileType;
	int _fileSize;
	int _length;
	int _bitrate;
	QString _encodedApplication;

	//Metadata
	QHash<Metadata, QString> _metadataHash;
	void insertMetadata(Metadata metadata, const QString & value);

	//Audio
	int _audioStreamCount;
	QHash<int, QString> _audioStreamHash;
	void insertAudioStream(int audioStreamId, AudioStream audioStream, const QString & value);

	//Video
	int _videoStreamCount;
	QHash<int, QString> _videoStreamHash;
	void insertVideoStream(int videoStreamId, VideoStream videoStream, const QString & value);

	//Text
	int _textStreamCount;
	QHash<int, QString> _textStreamHash;
	void insertTextStream(int textStreamId, TextStream textStream, const QString & value);

	//Network stream metadata
	QHash<NetworkStream, QString> _networkStreamHash;
	void insertNetworkStream(NetworkStream networkStream, const QString & value);
};

#endif	//MEDIAINFOFETCHER_H
