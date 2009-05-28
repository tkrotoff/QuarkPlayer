/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <quarkplayer-plugins/playlist/playlist_export.h>

#include <phonon/phononnamespace.h>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include <QtCore/QUuid>

class PlaylistCommandLineParser;

class QuarkPlayer;
class MediaInfoFetcher;
class MediaInfo;

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
class PLAYLIST_API PlaylistModel : public QAbstractItemModel {
	Q_OBJECT
public:

	static const int COLUMN_TRACK;
	static const int COLUMN_TITLE;
	static const int COLUMN_ARTIST;
	static const int COLUMN_ALBUM;
	static const int COLUMN_LENGTH;
	static const int COLUMN_FIRST;
	static const int COLUMN_LAST;

	PlaylistModel(QObject * parent, QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~PlaylistModel();

	static const int APPEND_FILES;

	/**
	 * Adds files to the model.
	 *
	 * Saves the new added files to the current playlist if needed.
	 * A file can also be a playlist file (.m3u, .pls...)
	 *
	 * The list of files can contain directory, addFiles() will go recursive to find all the files.
	 *
	 * @param files files to add to the model
	 * @param row location in the model where to add the files; if APPEND_FILES then append the files
	 */
	void addFiles(const QStringList & files, int row = APPEND_FILES);

	void addFiles(const QList<MediaInfo> & files, int row = APPEND_FILES);

	/**
	 * Plays the file at the given position.
	 */
	void play(int position);

	/**
	 * Loads a playlist file (.m3u, .pls...).
	 */
	void loadPlaylist(const QString & filename);

	/**
	 * Enqueues the file at the given position.
	 */
	void enqueue(int position);

	/** Sets the current item position inside the model. */
	void setPosition(int position);

	/** Gets the current item position inside the model. */
	int position() const;

	/** Returns the files inside the playlist. */
	const QList<MediaInfo> & files() const;

	/** Gets the filename given its index. */
	MediaInfo mediaInfo(const QModelIndex & index) const;


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
	///

public slots:

	/** Clears the model. */
	void clear();

	/** Loads the current playlist. */
	void loadCurrentPlaylist();

	/** Saves the current playlist to a file. */
	void saveCurrentPlaylist();

signals:

	void playlistLoaded(int timeElapsed);

	void playlistSaved(int timeElapsed);

private slots:

	void playInternal();

	void updateMediaInfo();

	void filesFound(const QStringList & files);

	void filesFound(const QList<MediaInfo> & files);

	void addFilesToCurrentPlaylist(const QStringList & files);

	void searchfinished(int timeElapsed);

	void allPluginsLoaded();

private:

	void insertFilesInsideTheModel(const QList<MediaInfo> & files, int row);

	enum TrackDisplayMode {
		TrackDisplayModeNormal,
		TrackDisplayModeWinamp
	};

	void clearInternal();

	QString currentPlaylist() const;

	QuarkPlayer & _quarkPlayer;

	/** Resolves the list of pending files for metadata/info. */
	MediaInfoFetcher * _mediaInfoFetcher;

	/**
	 * List of all the media (filenames) available in this QAbstractItemModel.
	 */
	QList<MediaInfo> _filenames;

	/**
	 * _mediaInfoFetcher is working or not (already resolving some metadatas or not).
	 * Pending row for meta data/info to be resolved.
	 */
	mutable int _mediaInfoFetcherRow;

	/** Current item position: the item/row/file that is currenlty selected inside the model/playlist. */
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

	int _nbFindFiles;

	/**
	 * The rows drag and droped internaly.
	 * This is needed for detecting we the current playlist row
	 * is being drag and droped.
	 */
	mutable QList<int> _dragAndDropRows;

	/**
	 * UUID from PluginInterface and PlaylistWidget.
	 *
	 * Necessary for loading and saving several playlist.
	 */
	QUuid _uuid;

	/**
	 * Position to play.
	 *
	 * @see playInternal()
	 */
	int _positionToPlay;

	PlaylistCommandLineParser * _commandLineParser;
};

#endif	//PLAYLISTMODEL_H
