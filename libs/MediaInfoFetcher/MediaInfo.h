/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <MediaInfoFetcher/mediainfofetcher_export.h>

#include <FileTypes/FileType.h>

#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QMetaType>
#include <QtCore/QVariant>

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
 * @see PlaylistParser
 * @author Tanguy Krotoff
 */
class MEDIAINFOFETCHER_API MediaInfo {
public:

	MediaInfo();

	/**
	 * Constructs a MediaInfo given a filename or a URL.
	 *
	 * @param fileName a media filename or a URL
	 */
	explicit MediaInfo(const QString & fileName);

	~MediaInfo();

	bool operator==(const MediaInfo & mediaInfo) const;

	/** Resets this class. */
	void clear();

	/** Tells if the metadata were fetched or not. */
	bool fetched() const;
	void setFetched(bool fetched);

	/** MediaInfo filename, can also be a URL. */
	QString fileName() const;
	/** Internal. */
	void setFileName(const QString & fileName);

	/** Helper function: determine if a given filename is a URL or not. */
	static bool isUrl(const QString & fileName);

	/** Gets the file type. */
	FileType fileType() const;
	void setFileType(FileType fileType);

	/**
	 * Gets the size of the file in kilobytes (KB).
	 *
	 * Stored inside an integer (32bits), this is gives us up to
	 * a size of 2147GB, I guess this is enough for the near future.
	 *
	 * @return file size in KB or -1 if failed
	 */
	int fileSize() const;
	void setFileSize(int kilobytes);

	/**
	 * Returns the duration/length of the file in a nice formatted way (i.e 03:45:02).
	 *
	 * @see TkTime::convertMilliseconds()
	 */
	QString durationFormatted() const;

	/**
	 * Returns the duration/length of the file in seconds, or -1.
	 *
	 * Qt uses abreviation Secs for seconds cf
	 * http://qt.nokia.com/doc/4.6/qtime.html#addSecs
	 */
	qint64 durationSecs() const;

	/**
	 * Returns the duration/length of the file in milliseconds, or -1.
	 *
	 * Qt uses abreviation MSecs for milliseconds cf
	 * http://qt.nokia.com/doc/4.6/qtime.html#addMSecs
	 * http://qt.nokia.com/doc/4.6/qtime.html#msecsTo
	 */
	qint64 durationMSecs() const;

	/** Sets the duration/length of the file in seconds. */
	void setDurationSecs(qint64 seconds);

	/** Sets the duration/length of the file in milliseconds. */
	void setDurationMSecs(qint64 milliseconds);

	/**
	 * CUE start/end INDEX info.
	 *
	 * Inside a CUE sheet file, INDEX is the position where the media should start to play.
	 *
	 * Numbers given are in milliseconds.
	 * Strings given are of the format:
	 * Format: [mm:ss:ff]
	 * mm:ss:ff - Starting time in minutes, seconds, and frames (75 frames/second)
	 * Example:
	 * 00:00:00
	 * 02:34:50
	 *
	 * @see http://digitalx.org/cuesheetsyntax.php#indx
	 */
	static const int CUE_INDEX_INVALID = -1;
	void setCueStartIndex(const QString & cueIndex);
	void setCueEndIndex(const QString & cueIndex);
	QString cueStartIndexFormatted() const;
	QString cueEndIndexFormatted() const;
	qint64 cueStartIndex() const;
	qint64 cueEndIndex() const;

	/**
	 * Gets the overall bitrate = audio bitrate + video bitrate if any.
	 *
	 * Uses kilobits per second (kbps or kbit/s).
	 *
	 * @see http://en.wikipedia.org/wiki/Bit_rate
	 *
	 * Seems like Bitrate is a better name than BitRate:
	 * Bitrate (Java): 3000 answers
	 * http://www.google.com/codesearch?as_q=Bitrate&btnG=Search+Code&hl=en&as_lang=java&as_case=y
	 * BitRate (Java): 1000 answers
	 * http://www.google.com/codesearch?as_q=BitRate&btnG=Search+Code&hl=en&as_lang=java&as_case=y
	 *
	 * @return the overall bitrate of the media file in kbps or -1 if failed
	 */
	int bitrate() const;
	void setBitrate(int kbps);

	/** Gets the application used to encode the file. */
	QString encodedApplication() const;
	void setEncodedApplication(const QString & encodedApplication);

	/**
	 * MetaData.
	 *
	 * @see http://en.wikipedia.org/wiki/Metadata
	 *
	 * Seems like MetaData is a better name than Metadata:
	 * MetaData (Java): 90,500 answers
	 * http://www.google.com/codesearch?as_q=MetaData&btnG=Search+Code&hl=en&as_lang=java&as_case=y
	 * Metadata (Java): 81,200 answers
	 * http://www.google.com/codesearch?as_q=Metadata&btnG=Search+Code&hl=en&as_lang=java&as_case=y
	 */
	enum MetaData {
		/**
		 * int
		 * Returns 0 if no track number.
		 */
		TrackNumber,

		/**
		 * int
		 * Returns 0 if no disc number
		 */
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

		/** QDate */
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

		/** QUrl */
		URL,

		/** QString */
		EncodedBy,

		/** QString */
		AlbumArtistSort,

		/**
		 * FIXME QString -> QUuid
		 * Cannot be a QUuid, see http://bugreports.qt.nokia.com/browse/QTBUG-373
		 *
		 * @see http://musicbrainz.org/doc/ArtistID
		 */
		MusicBrainzArtistId,

		/**
		 * FIXME QString -> QUuid
		 * For MusicBrainz, release = album.
		 * Cannot be a QUuid, see http://bugreports.qt.nokia.com/browse/QTBUG-373
		 *
		 * @see http://musicbrainz.org/doc/ReleaseID
		 */
		MusicBrainzReleaseId,

		/**
		 * FIXME QString -> QUuid
		 * Cannot be a QUuid, see http://bugreports.qt.nokia.com/browse/QTBUG-373
		 *
		 * @see http://musicbrainz.org/doc/TrackID
		 */
		MusicBrainzTrackId,

		/**
		 * QString
		 *
		 * Contains characters and numbers.
		 * @see http://en.wikipedia.org/wiki/Amazon_Standard_Identification_Number
		 */
		AmazonASIN,

		/**
		 * Beats per minute is a unit used as a measure of tempo in music.
		 *
		 * int
		 * @see http://en.wikipedia.org/wiki/Beats_per_minute
		 */
		BPM
	};

	QVariant metaDataValue(MetaData metaData) const;
	void setMetaData(MetaData metaData, const QVariant & value);

	/**
	 * Associates a QVariant with this MediaInfo.
	 *
	 * This function was added in order to easily extend MediaInfo
	 * without inheriting from it (overly complex).
	 *
	 * @param key
	 * @param value the meaning of value is up to the user
	 */
	void setExtendedMetaData(const QString & key, const QVariant & value);
	QVariant extendedMetaData(const QString & key) const;

	//Audio
	enum AudioStream {
		/**
		 * int
		 * Uses kilobits per second (kbps or kbit/s).
		 */
		AudioBitrate,

		/** QString */
		AudioBitrateMode,

		/**
		 * Defines the number of samples per second taken from a continuous signal to make a discrete signal.
		 *
		 * int
		 * Uses kilohertz (kHz).
		 *
		 * @see http://en.wikipedia.org/wiki/Sample_rate
		 */
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
	QVariant audioStreamValue(int audioStreamId, AudioStream audioStream) const;
	void insertAudioStream(int audioStreamId, AudioStream audioStream, const QVariant & value);

	//Video
	enum VideoStream {
		/** int */
		VideoBitrate,

		/**
		 * QSize
		 * QSize(int width, int height) in pixels.
		 */
		VideoResolution,

		/**
		 * Frame rate, or frame frequency, is the frequency (rate) at which an imaging
		 * device produces unique consecutive images called frames.
		 *
		 * int
		 * Uses frames per second (FPS).
		 * @see http://en.wikipedia.org/wiki/Frame_rate
		 */
		VideoFrameRate,

		/** QString */
		VideoFormat,

		/** QString */
		VideoCodec,

		/** QString */
		VideoEncodedLibrary
	};

	int videoStreamCount() const;
	QVariant videoStreamValue(int videoStreamId, VideoStream videoStream) const;
	void insertVideoStream(int videoStreamId, VideoStream videoStream, const QVariant & value);

	//Text
	enum TextStream {
		/** QString */
		TextFormat,

		/** QString */
		TextLanguage
	};

	int textStreamCount() const;
	QVariant textStreamValue(int textStreamId, TextStream textStream) const;
	void insertTextStream(int textStreamId, TextStream textStream, const QVariant & value);

	//Stream
	enum NetworkStream {
		/** QString */
		StreamName,

		/** QString */
		StreamGenre,

		/** QUrl */
		StreamWebsite,

		/** QUrl */
		StreamURL
	};

	QVariant networkStreamValue(NetworkStream networkStream) const;
	void insertNetworkStream(NetworkStream networkStream, const QVariant & value);

private:

	/** Factorization code: parses a CUE index string. */
	static qint64 parseCueIndexString(const QString & cueIndexString);

	/** Factorization code: generates a CUE index string given a CUE index number. */
	static QString cueIndexFormatted(qint64 cueIndex);

	bool _fetched;
	QString _fileName;
	FileType _fileType;
	int _fileSize;

	/** Duration/length of the file in milliseconds. */
	qint64 _duration;

	int _bitrate;
	QString _encodedApplication;

	/**
	 * CUE feature: position in milliseconds where to start the file.
	 * CUE_INDEX_INVALID if unset.
	 */
	qint64 _cueStartIndex;
	qint64 _cueEndIndex;

	/** MetaData. */
	QHash<MetaData, QVariant> _metaDataHash;

	/** Audio. */
	int _audioStreamCount;
	QHash<int, QVariant> _audioStreamHash;

	/** Video. */
	int _videoStreamCount;
	QHash<int, QVariant> _videoStreamHash;

	/** Text. */
	int _textStreamCount;
	QHash<int, QVariant> _textStreamHash;

	/** Network stream metadata. */
	QHash<NetworkStream, QVariant> _networkStreamHash;

	/** Extended metadata. */
	QHash<QString, QVariant> _extendedMetaData;
};

Q_DECLARE_METATYPE(MediaInfo);
Q_DECLARE_METATYPE(QList<MediaInfo>);

#include <QtCore/QList>

/**
 * A list of MediaInfo.
 */
typedef QList<MediaInfo> MediaInfoList;

#endif	//MEDIAINFO_H
