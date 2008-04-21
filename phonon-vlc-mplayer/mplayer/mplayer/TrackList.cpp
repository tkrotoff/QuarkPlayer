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

#include "TrackList.h"

#include <QtCore/QRegExp>

TrackList::TrackList() {
	clear();
}

TrackList::~TrackList() {
}

void TrackList::clear() {
	tm.clear();
}

void TrackList::addLang(int ID, QString lang) {
	tm[ID].setLang(lang);
	tm[ID].setID(ID);
}

void TrackList::addName(int ID, QString name) {
	tm[ID].setName(name);
	tm[ID].setID(ID);
}

void TrackList::addFilename(int ID, QString filename) {
	tm[ID].setFilename(filename);
	tm[ID].setID(ID);
}

void TrackList::addDuration(int ID, double duration) {
	tm[ID].setDuration(duration);
	tm[ID].setID(ID);
}

void TrackList::addChapters(int ID, int n) {
	tm[ID].setChapters(n);
	tm[ID].setID(ID);
}

void TrackList::addAngles(int ID, int n) {
	tm[ID].setAngles(n);
	tm[ID].setID(ID);
}

void TrackList::addID(int ID) {
	tm[ID].setID(ID);
}

int TrackList::numItems() const {
	return tm.count();
}

bool TrackList::existsItemAt(int n) const {
	return ((n > 0) && (n < numItems()));
}

TrackData TrackList::itemAt(int n) const {
	return tm.values()[n];
}

TrackData TrackList::item(int ID) const {
	return tm[ID];
}

int TrackList::find(int ID) const {
	for (int n = 0; n < numItems(); n++) {
		if (itemAt(n).ID() == ID) return n;
	}
	return -1;
}

int TrackList::findLang(const QString & lang) const {
	qDebug("TrackList::findLang: '%s'", lang.toUtf8().data());
	QRegExp rx(lang);

	int res_id = -1;

	for (int n = 0; n < numItems(); n++) {
		qDebug("TrackList::findLang: lang #%d '%s'", n, itemAt(n).lang().toUtf8().data());
		if (rx.indexIn(itemAt(n).lang() ) > -1) {
			qDebug("TrackList::findLang: found preferred lang!");
			res_id = itemAt(n).ID();
			break;
		}
	}

	return res_id;
}

int TrackList::lastID() {
	int key = -1;
	for (int n=0; n < numItems(); n++) {
		if (itemAt(n).ID() > key)
			key = itemAt(n).ID();
	}
	return key;
}

bool TrackList::existsFilename(QString name) {
	for (int n=0; n < numItems(); n++) {
		if (itemAt(n).filename() == name)
			return TRUE;
	}
	return FALSE;
}
