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

#ifndef TRACKLIST_H
#define TRACKLIST_H

#include "TrackData.h"

#include <QtCore/QMap>

class TrackList {
public:

	TrackList();
	~TrackList();

	void clear();

	void addLang(int ID, QString lang);
	void addName(int ID, QString name);
	void addFilename(int ID, QString filename);
	void addDuration(int ID, double duration);
	void addChapters(int ID, int n);
	void addAngles(int ID, int n);
	void addID(int ID);

	int numItems() const;
	bool existsItemAt(int n) const;

	TrackData itemAt(int n) const;
	TrackData item(int ID) const;
	int find(int ID) const;

	int findLang(const QString & lang) const;

	// A mess for getting sub files...
	int lastID();
	bool existsFilename(QString name);

protected:
	typedef QMap <int, TrackData> TrackMap;
	TrackMap tm;
};

#endif	//TRACKLIST_H
