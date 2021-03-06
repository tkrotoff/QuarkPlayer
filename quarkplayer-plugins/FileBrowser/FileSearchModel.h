/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef FILESEARCHMODEL_H
#define FILESEARCHMODEL_H

#include <quarkplayer-plugins/FileBrowser/FileBrowserExport.h>

#include <QtGui/QIcon>
#include <QtGui/QFileIconProvider>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QStringList>
#include <QtCore/QEventLoop>
#include <QtCore/QUuid>

class FindFiles;
class MediaInfoFetcher;
class MediaInfo;
class FileSearchItem;

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
class FILEBROWSER_API FileSearchModel : public QAbstractItemModel {
	Q_OBJECT
public:

	FileSearchModel(QObject * parent);

	~FileSearchModel();

	/**
	 * Sets the file extensions to match when the search is performed.
	 *
	 * The unmatching file will be discarsed.
	 *
	 * @see FindFiles::setExtensions()
	 * @param extensions file extensions to match ex: "what is love.mp3" matches "mp3" extension
	 */
	void setSearchExtensions(const QStringList & extensions);

	/**
	 * Sets the file extensions for which "advanced" tooltips will be shown.
	 *
	 * Why?
	 * .mp3, .flac, .avi files can get "advanced" informations, not the other files.
	 *
	 * @param extensions file extensions to match for "advanced" tooltips ex: "what is love.mp3" matches "mp3" extension
	 */
	void setToolTipExtensions(const QStringList & extensions);

	/**
	 * Sets the root search path.
	 *
	 * Nothing fancy here, only used internally for the tooltips.
	 */
	void setRootSearchPath(const QString & rootSearchPath);

	QFileInfo fileInfo(const QModelIndex & index) const;

	/**
	 * Resets the model (and the view).
	 *
	 * @see QAbstractItemModel::reset()
	 */
	void reset();

	/**
	 * Searches a path for files given a pattern and file extensions.
	 *
	 * @see FindFiles
	 */
	void search(const QString & path, const QRegExp & pattern, int filesFoundLimit, bool recursiveSearch);

	void stop();

	/**
	 * @name Inherited from QAbstractItemModel
	 * @{
	 */

	int columnCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex & index) const;
	int rowCount(const QModelIndex & parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex & index) const;
	bool hasChildren(const QModelIndex & parent = QModelIndex()) const;

	bool canFetchMore(const QModelIndex & parent) const;
	void fetchMore(const QModelIndex & parent);

	QMimeData * mimeData(const QModelIndexList & indexes) const;
	QStringList mimeTypes() const;
	Qt::DropActions supportedDropActions() const;

	/** @} */

signals:

	void searchFinished(int timeElapsed);

private slots:

	void filesFound(const QStringList & files);

	void updateMediaInfo(const MediaInfo & mediaInfo);

private:

	FileSearchItem * item(const QModelIndex & index) const;

	QModelIndex index(const FileSearchItem * item) const;

	FindFiles * _findFiles;

	/** Resolves the list of pending files for metadata/info. */
	MediaInfoFetcher * _mediaInfoFetcher;

	/**
	 * List of all the media (filenames) available in this QAbstractItemModel.
	 */
	FileSearchItem * _rootItem;

	FileSearchItem * _currentParentItem;

	/**
	 * _mediaInfoFetcher is working or not (already resolving some metadatas or not).
	 * Pending index (row + column) for meta data/info to be resolved.
	 */
	mutable QModelIndex _mediaInfoFetcherIndex;

	QStringList _searchExtensions;

	QStringList _toolTipExtensions;

	/** Used internally for tooltips. */
	QString _rootSearchPath;

	/** Icon provider: gives us the icons matching a file extension. */
	QFileIconProvider _iconProvider;

	/**
	 * Saves the icons inside a cache system given a filename extension (mp3, ogg, avi...).
	 *
	 * This is for optimization purpose
	 * Key = filename extension or empty if a directory
	 * Value = icon matching the extension
	 */
	static QHash<QString, QIcon> _iconsCache;
};

#endif	//FILESEARCHMODEL_H
