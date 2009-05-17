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

#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <mediainfowindow/mediainfowindow_export.h>

#include <filetypes/FileType.h>

#include <QtCore/QString>
#include <QtCore/QHash>

/**
 * Infos and other metadata associated with a media/track.
 *
 * MediaInfo is a "stupid" data structure that stores infos and metadata
 * associated with a media (e.g a track).
 * MediaInfo is empty by default and should be used in association with
 * MediaInfoFetcher that will fetch all the datas and put them into MediaInfo.
 *
 * Each item inside the playlist is a MediaInfo object.
 * Thus MediaInfo should be a simple and lightweight class.
 *
 * @see MediaInfoFetcher
 * @author Tanguy Krotoff
 */
class MEDIAINFOWINDOW_API MediaInfo {
public:

	MediaInfo();

	MediaInfo(const QString & filename);

	~MediaInfo();

	void clear();

	/** Tells if the metadata were fetched or not. */
	bool fetched() const;
	void setFetched(bool fetched);

	/** FileName. */
	QString fileName() const;
	void setFileName(const QString & fileName);

	/** If the fileName is a URL or not. */
	bool isUrl() const;
	void setUrl(bool url);

	/** Gets the file type. */
	FileType fileType() const;
	void setFileType(FileType fileType);

	/** Gets the size of the file in kbytes. */
	QString fileSize() const;
	void setFileSize(int fileSize);

	/** Returns the length of the file in a nice formatted way (i.e 03:45:02). */
	QString lengthFormatted() const;
	/** Returns the length of the file in seconds, or -1. */
	int lengthSeconds() const;
	/** Sets the length of the file in seconds. */
	void setLength(int length);

	/** Gets the overall bitrate = audio bitrate + video bitrate if any. */
	QString bitrate() const;
	void setBitrate(int bitrate);

	/** Gets the application used to encode the file. */
	QString encodedApplication() const;
	void setEncodedApplication(const QString & encodedApplication);

	//Metadata
	enum Metadata {
		/** int */
		TrackNumber,
		/** int */
		DiscNumber,
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
		EncodedBy,
		/** QString */
		AlbumArtistSort,
		/** QString */
		MusicBrainzArtistId,
		/** For MusicBrainz, release = album. */
		MusicBrainzReleaseId,
		/** QString */
		MusicBrainzTrackId,
		/** QString */
		AmazonASIN,
		/** int */
		BPM
	};

	QString metadataValue(Metadata metadata) const;
	void insertMetadata(Metadata metadata, const QString & value);

	//Audio
	enum AudioStream {
		/** int */
		AudioBitrate,
		/** QString */
		AudioBitrateMode,
		/** int */
		AudioSampleRate,
		/** int */
		AudioBitsPerSample,
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
	void insertAudioStream(int audioStreamId, AudioStream audioStream, const QString & value);

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
	void insertVideoStream(int videoStreamId, VideoStream videoStream, const QString & value);

	//Text
	enum TextStream {
		/** QString */
		TextFormat,
		/** QString */
		TextLanguage
	};

	int textStreamCount() const;
	QString textStreamValue(int textStreamId, TextStream textStream) const;
	void insertTextStream(int textStreamId, TextStream textStream, const QString & value);

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
	void insertNetworkStream(NetworkStream networkStream, const QString & value);

private:

	bool _fetched;

	//General
	QString _fileName;
	bool _isUrl;
	FileType _fileType;
	int _fileSize;
	int _length;
	int _bitrate;
	QString _encodedApplication;

	//Metadata
	QHash<Metadata, QString> _metadataHash;

	//Audio
	int _audioStreamCount;
	QHash<int, QString> _audioStreamHash;

	//Video
	int _videoStreamCount;
	QHash<int, QString> _videoStreamHash;

	//Text
	int _textStreamCount;
	QHash<int, QString> _textStreamHash;

	//Network stream metadata
	QHash<NetworkStream, QString> _networkStreamHash;
};

#endif	//MEDIAINFO_H
