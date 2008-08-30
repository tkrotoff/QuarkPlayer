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

#ifndef PLAYLISTFILTER_H
#define PLAYLISTFILTER_H

#include <QtGui/QSortFilterProxyModel>

class PlaylistModel;

/**
 * Sorts/filters the playlist given a string.
 *
 * @author Tanguy Krotoff
 */
class PlaylistFilter : public QSortFilterProxyModel {
	Q_OBJECT
public:

	PlaylistFilter(QObject * parent, PlaylistModel * playlistModel);

	void setFilter(const QString & filter);

	QModelIndex currentIndex() const;

public slots:

	void play(const QModelIndex & index);

	void playNextTrack();

	void playPreviousTrack();

	void enqueueNextTrack();

	void setShuffle(bool shuffle);

	void setRepeat(bool repeat);

signals:

	void filterChanged();

private:

	QModelIndex nextTrack() const;

	QModelIndex previousTrack() const;

	void setCurrentIndex(const QModelIndex & index);
	int modelPosition(const QModelIndex & index);

	bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const;

	QString _filter;

	PlaylistModel * _playlistModel;

	mutable int _position;

	bool _shuffle;

	bool _repeat;
};

#endif	//PLAYLISTFILTER_H
