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

class Track;

class QuarkPlayer;
class MediaInfoFetcher;

/**
 * Playlist model.
 *
 * Internally, PlaylistModel keeps the entire playlist in memory
 * via <pre>QList<Track> _filenames</pre>.
 * This is compulsary since the user can perform some searches
 * inside the playlist via the search ToolBar.
 * <pre>QList<Track> _filenames</pre> should be memory efficient and thus the
 * Track class is very minimalist.
 *
 * @author Tanguy Krotoff
 */
class PlaylistModel : public QAbstractItemModel {
	Q_OBJECT
public:

	static const int COLUMN_TRACK;
	static const int COLUMN_TITLE;
	static const int COLUMN_ARTIST;
	static const int COLUMN_ALBUM;
	static const int COLUMN_LENGTH;
	static const int COLUMN_FIRST;
	static const int COLUMN_LAST;

	PlaylistModel(QObject * parent, QuarkPlayer & quarkPlayer);

	~PlaylistModel();

	/**
	 * Adds files to the model.
	 *
	 * Saves the new added files to the current playlist if needed.
	 *
	 * The list of files can contain directory, addFiles() will go recursive to find all the files.
	 *
	 * @param files files to add to the model
	 * @param row location in the model where to add the files; if -1 then append the files
	 */
	void addFiles(const QStringList & files, int row = -1);

	/**
	 * Plays the file at the given position.
	 */
	void play(int position);

	/**
	 * Enqueues the file at the given position.
	 */
	void enqueue(int position);

	/** Sets the current item position inside the model. */
	void setPosition(int position);

	/** Gets the current item position inside the model. */
	int position() const;

	/** Returns the files displayed in the playlist. */
	QStringList fileNames() const;

	/** Gets the filename given its index. */
	QString filename(const QModelIndex & index) const;

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

	/** Clears the model. */
	void clear();

private slots:

	void updateMediaInfo();

	void loadCurrentPlaylist();

	void filesFound(const QStringList & files);

	void searchfinished(int timeElapsed);

private:

	/** Saves the current playlist to a file. */
	void saveCurrentPlaylist();

	void clearInternal();

	QuarkPlayer & _quarkPlayer;

	/** Resolves the list of pending files for metadata/info. */
	MediaInfoFetcher * _mediaInfoFetcher;

	/**
	 * List of all the media (filenames) available in this QAbstractItemModel.
	 */
	QList<Track> _filenames;

	/**
	 * _mediaInfoFetcher is working or not (already resolving some metadatas or not).
	 * Pending row for meta data/info to be resolved.
	 */
	mutable int _mediaInfoFetcherRow;

	int _position;

	/**
	 * Trick: row where to insert new files.
	 *
	 * I have to do that because of Qt slot/signal limitation :/
	 * this works with filesFound() slot defined above.
	 *
	 * @see filesFound()
	 * @see http://lists.trolltech.com/qt-interest/1998-01/thread00097-0.html
	 */
	int _rowWhereToInsertFiles;

	/**
	 * If the current playlist has been modified or not.
	 *
	 * If yes this means we have to save the current playlist.
	 */
	bool _currentPlaylistModified;
};

#endif	//PLAYLISTMODEL_H
