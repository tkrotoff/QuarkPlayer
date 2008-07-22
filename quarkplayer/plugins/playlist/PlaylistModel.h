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

#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <phonon/phononnamespace.h>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include <QtCore/QString>

namespace Phonon {
	class MediaObject;
}

class Track;

class QuarkPlayer;

/**
 * Playlist model.
 *
 * @author Tanguy Krotoff
 */
class PlaylistModel : public QAbstractItemModel {
	Q_OBJECT
public:

	PlaylistModel(QObject * parent, QuarkPlayer & quarkPlayer);

	~PlaylistModel();

	void highlightItem(int row);

	void addFiles(const QStringList & files, int row = -1);

	/** Returns the files displayed in the playlist. */
	QStringList files() const;

	//Inherited from QAbstractItemModel
	int columnCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex & index) const;
	int rowCount(const QModelIndex & parent = QModelIndex()) const;
	bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

	Qt::ItemFlags flags(const QModelIndex & index) const;

	bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);
	QMimeData * mimeData(const QModelIndexList & indexes) const;
	QStringList mimeTypes() const;
	Qt::DropActions supportedDropActions() const;

public slots:

	void clear();

	void play(const QModelIndex & index);

	void playNextTrack();

	void playPreviousTrack();

	void setShuffle(bool shuffle);

	void setRepeat(bool repeat);

private slots:

	void metaStateChanged(Phonon::State newState, Phonon::State oldState);

	void stateChanged(Phonon::State newState, Phonon::State oldState);

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

	void enqueueNextTrack();

private:

	void saveCurrentPlaylist() const;

	void updateRow(int row);

	QuarkPlayer & _quarkPlayer;

	/** Resolves the list of pending files for meta data/info. */
	Phonon::MediaObject * _metaObjectInfoResolver;

	/** List of all the media available in this QAbstractItemModel. */
	QList<Track> _mediaSources;

	/** List of pending files for meta data/info to be resolved. */
	QList<QString> _filesInfoResolver;

	int _position;

	bool _shuffle;

	bool _repeat;
};

#endif	//PLAYLISTMODEL_H
