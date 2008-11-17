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

	/**
	 * Starts info fetching given a media source.
	 *
	 * mediaSource cannot be a URL, only a file.
	 * If mediaSource is a URL then use start(Phonon::MediaObject * mediaObject)
	 *
	 * @param mediaSource Phonon media source
	 */
	void start(const Phonon::MediaSource & mediaSource);

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

	QString fileName() const;

	bool isUrl() const;

	FileType fileType() const;

	/** Returns the track number; if there is no track number set, this will return 0. */
	QString trackNumber() const;

	QString title() const;
	QString artist() const;
	QString album() const;
	QString year() const;
	QString genre() const;
	QString comment() const;

	/** Returns the length of the file in seconds. */
	QString length() const;

	/**
	 * Returns the most appropriate bit rate for the file in kbit/s.
	 *
	 * For constant bitrate formats this is simply the bitrate of the file.
	 * For variable bitrate formats this is either the average or nominal bitrate.
	 *
	 * @return bitrate or -1
	 */
	QString bitrate() const;

	/** Returns the sample rate in Hz. */
	QString sampleRate() const;

	/** Returns the number of audio channels. */
	QString channels() const;

	/** Returns the size of the file in kbytes. */
	QString fileSize();

	QString streamName() const;
	QString streamGenre() const;
	QString streamWebsite() const;
	QString streamUrl() const;

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

	/** Determines file type from the file extension. */
	void determineFileTypeFromExtension();

	Phonon::MediaSource _mediaSource;

	bool _fetched;

	/** Resolves media metadata/info. */
	Phonon::MediaObject * _metaObjectInfoResolver;

	QString _filename;
	bool _isUrl;

	FileType _fileType;

	int _trackNumber;
	QString _title;
	QString _artist;
	QString _album;
	int _year;
	QString _genre;
	QString _comment;

	int _length;
	int _bitrate;
	int _sampleRate;
	int _channels;

	QString _streamName;
	QString _streamGenre;
	QString _streamWebsite;
	QString _streamUrl;
};

#endif	//MEDIAINFOFETCHER_H
