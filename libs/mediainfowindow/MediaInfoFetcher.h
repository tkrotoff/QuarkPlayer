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

#include <phonon/phononnamespace.h>

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
	 * Starts info fetching given a media filename.
	 *
	 * @param filename media filename
	 */
	void start(const QString & filename);

	/** Tells if the metadata were fetched or not. */
	bool fetched() const;

	/** Returns the track number; if there is no track number set, this will return 0. */
	int trackNumber() const;

	QString title() const;
	QString artist() const;
	QString album() const;
	int year() const;
	QString genre() const;
	QString comment() const;

	/** Returns the length of the file in seconds. */
	int length() const;

	/**
	 * Returns the most appropriate bit rate for the file in kb/s.
	 *
	 * For constant bitrate formats this is simply the bitrate of the file.
	 * For variable bitrate formats this is either the average or nominal bitrate.
	 */
	int bitrate() const;

	/** Returns the size of the file in octets. */
	long fileSize() const;

	/** Returns the number of audio channels. */
	int channels() const;

	/** Returns the sample rate in Hz. */
	int sampleRate() const;

	QString streamName() const;
	QString streamGenre() const;
	QString streamWebsite() const;
	QString streamURL() const;

signals:

	void fetched();

private slots:

	void metaStateChanged(Phonon::State newState, Phonon::State oldState);

private:

	void startPhononResolver();

	void startTagLibResolver();

	QString _filename;

	bool _fetched;

	/** Resolves media metadata/info. */
	Phonon::MediaObject * _metaObjectInfoResolver;

	int _trackNumber;
	QString _title;
	QString _artist;
	QString _album;
	int _year;
	QString _genre;
	QString _comment;

	int _length;
	int _bitrate;

	long _fileSize;

	int _channels;
	int _sampleRate;

	QString _streamName;
	QString _streamGenre;
	QString _streamWebsite;
	QString _streamURL;
};

#endif	//MEDIAINFOFETCHER_H
