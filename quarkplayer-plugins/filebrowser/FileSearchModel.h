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

#ifndef FILESEARCHMODEL_H
#define FILESEARCHMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QStringList>

#include <QtGui/QIcon>

class Track;

class FindFiles;
class MediaInfoFetcher;

class QFileIconProvider;
class QFileInfo;
class QRegExp;

/**
 * Replacement for QDirModel or QFileSystemModel,
 * simple flat model populated via a recursive file search.
 *
 * Based on FindFiles
 *
 * FIXME a lot can be factorized with PlaylistModel
 * Specially _mediaInfoFetcherRow, Track class, MediaInfoFetcher * _mediaInfoFetcher;
 * QList<Track> _filenames, updateMediaInfo()...
 * All this is a clean copy-paste from PlaylistModel
 * but still code factorization is GOOD :)
 *
 * FIXME due to a Qt bug, tooltip refresh is not perfect :/
 *
 * @see QDirModel
 * @see QFileSystemModel
 * @see FindFiles
 * @see PlaylistModel
 * @author Tanguy Krotoff
 */
class FileSearchModel : public QAbstractItemModel {
	Q_OBJECT
public:

	static const int COLUMN_FILENAME;
	static const int COLUMN_PATH;
	static const int COLUMN_FIRST;
	static const int COLUMN_LAST;

	FileSearchModel(QObject * parent);

	~FileSearchModel();

	QFileInfo fileInfo(const QModelIndex & index) const;

	void setIconProvider(QFileIconProvider * provider);

	/**
	 * Searches a path for files given a pattern and file extensions.
	 *
	 * @see FindFiles
	 */
	void search(const QString & path, const QRegExp & pattern, const QStringList & extensions);

	void stop();

	//Inherited from QAbstractItemModel
	int columnCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex & index) const;
	int rowCount(const QModelIndex & parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex & index) const;

	QMimeData * mimeData(const QModelIndexList & indexes) const;
	QStringList mimeTypes() const;
	Qt::DropActions supportedDropActions() const;

signals:

	void searchFinished(int timeElapsed);

private slots:

	void filesFound(const QStringList & files);

	void updateMediaInfo();

private:

	void clearInternal();

	FindFiles * _findFiles;

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

	QFileIconProvider * _iconProvider;

	/**
	 * Saves the icons inside a cache system given a filename extension (mp3, ogg, avi...).
	 *
	 * This is for optimization purpose.
	 * Used with QFileIconProvider.
	 *
	 * Key = filename extension or empty if a directory
	 * Value = icon matching the extension
	 */
	static QHash<QString, QIcon> _iconsCache;
};

#endif	//FILESEARCHMODEL_H
