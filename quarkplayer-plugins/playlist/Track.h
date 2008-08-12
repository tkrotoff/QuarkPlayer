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

#ifndef TRACK_H
#define TRACK_H

#include <phonon/mediasource.h>

#include <QtCore/QString>

/**
 * Track info and meta datas.
 *
 * Each item inside the playlist is a Track object.
 * Thus Track should be a simple and lightweight class.
 *
 * @author Tanguy Krotoff
 */
class Track {
public:

	explicit Track(const QString & filename);

	Track(const Track & track);

	~Track();

	Track & operator=(const Track & right);

	/** Comparaison based on filenames only. */
	int operator==(const Track & right);

	QString fileName() const;
	Phonon::MediaSource mediaSource() const;

	void setTrackNumber(const QString & trackNumber);
	QString trackNumber() const;

	void setTitle(const QString & title);
	QString title();

	void setArtist(const QString & artist);
	QString artist() const;

	void setAlbum(const QString & album);
	QString album() const;

	void setLength(const QString & length);
	QString length() const;

	void setMediaDataResolved(bool resolved);
	bool mediaDataResolved() const;

private:

	void copy(const Track & track);

	void setMediaSource(const Phonon::MediaSource & mediaSource);

	QString convertMilliseconds(qint64 totalTime) const;

	QString _filename;

	QString _trackNumber;
	QString _title;
	QString _artist;
	QString _album;
	QString _length;
	bool _resolved;
};

#endif	//TRACK_H
