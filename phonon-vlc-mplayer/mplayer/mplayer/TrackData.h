/*
 * VLC and MPlayer backends for the Phonon library
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef TRACKDATA_H
#define TRACKDATA_H

#include <QtCore/QString>

/**
 * Class to store audios, subtitles, titles, chapters...
 */
class TrackData {
public:

	TrackData();
	~TrackData();

	void setLang( const QString & l ) { _lang = l; };
	void setName( const QString & n ) { _name = n; };
	void setFilename( const QString & f ) { _filename = f; };
	void setDuration( double d ) { _duration = d; };
	void setChapters( int n ) { _chapters = n; };
	void setAngles( int n ) { _angles = n; };
	void setID( int id ) { _ID = id; };

	QString lang() const { return _lang; };
	QString name() const { return _name; };
	QString filename() const { return _filename; };
	double duration() const { return _duration; };
	int chapters() const { return _chapters; };
	int angles() const { return _angles; };
	int ID() const { return _ID; };

	QString displayName() const;

private:

	static QString formatTime(int secs);

	/* Language code: es, en, etc. */
	QString _lang;

	/* spanish, english... */
	QString _name;

	/* In case of subtitles files (*.srt, sub...) */
	QString _filename;

	/* For dvd titles */
	double _duration;
	int _chapters;
	int _angles;

	int _ID;
};

#endif	//TRACKDATA_H
