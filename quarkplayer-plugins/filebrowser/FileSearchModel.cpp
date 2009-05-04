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

#include "FileSearchModel.h"

#include "FileSearchItem.h"

#include <mediainfowindow/MediaInfoFetcher.h>

#include <filetypes/FileTypes.h>

#include <tkutil/FindFiles.h>
#include <tkutil/TkFile.h>

#include <QtGui/QtGui>

#include <QtCore/QtConcurrentRun>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

//For INT_MAX
#include <climits>

const int FileSearchModel::COLUMN_FILENAME = 0;
const int FileSearchModel::COLUMN_FIRST = COLUMN_FILENAME;
const int FileSearchModel::COLUMN_LAST = COLUMN_FILENAME;

static const int COLUMN_COUNT = 1;

static const int POSITION_INVALID = -1;

QHash<QString, QIcon> FileSearchModel::_iconsCache;

FileSearchModel::FileSearchModel(QObject * parent)
	: QAbstractItemModel(parent) {

	_rootItem = NULL;
	_currentParentItem = NULL;

	reset();

	_findFiles = NULL;

	//Info fetcher
	_mediaInfoFetcher = new MediaInfoFetcher(this);
	connect(_mediaInfoFetcher, SIGNAL(fetched()), SLOT(updateMediaInfo()));
}

FileSearchModel::~FileSearchModel() {
	reset();
}

void FileSearchModel::setSearchExtensions(const QStringList & extensions) {
	_searchExtensions = extensions;
}

void FileSearchModel::setToolTipExtensions(const QStringList & extensions) {
	_toolTipExtensions = extensions;
}

int FileSearchModel::columnCount(const QModelIndex & parent) const {
	Q_UNUSED(parent);
	return COLUMN_COUNT;
}

QVariant FileSearchModel::headerData(int section, Qt::Orientation orientation, int role) const {
	QVariant tmp;

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch (section) {
		case COLUMN_FILENAME:
			tmp = tr("Name");
			break;
		}
	}

	return tmp;
}

QVariant FileSearchModel::data(const QModelIndex & index, int role) const {
	QVariant tmp;

	if (!index.isValid() || index.model() != this) {
		return tmp;
	}

	int row = index.row();
	int column = index.column();

	FileSearchItem * item = static_cast<FileSearchItem *>(index.internalPointer());
	if (!item) {
		qWarning() << __FUNCTION__ << "Error: item is NULL";
		return tmp;
	}
	const MediaInfo & mediaInfo(item->mediaInfo());
	QString filename(mediaInfo.fileName());

	switch (role) {
	case Qt::DisplayRole: {
		switch (column) {
		case COLUMN_FILENAME:
			tmp = QFileInfo(filename).fileName();
			break;
		}
		break;
	}

	case Qt::DecorationRole: {
		switch (column) {
		case COLUMN_FILENAME:
			//This is too slow:
			//re-creates an icon for each file
			//We want a cache system -> way faster!
			//tmp = _iconProvider.icon(QFileInfo(filename));

			QFileInfo fileInfo(filename);
			QString ext(fileInfo.suffix());
			if (!_iconsCache.contains(ext)) {
				_iconsCache[ext] = _iconProvider.icon(fileInfo);
			}
			tmp = _iconsCache.value(ext);
			break;
		}
		break;
	}

	case Qt::ToolTipRole: {
		switch (column) {
		case COLUMN_FILENAME:
			QFileInfo fileInfo(filename);
			if (!fileInfo.isDir() && _toolTipExtensions.contains(fileInfo.suffix(), Qt::CaseInsensitive)) {
				if (mediaInfo.fetched()) {
					tmp = filename + "<br>" +
						tr("Title:") + "</b> <b>" + mediaInfo.metadataValue(MediaInfo::Title) + "</b><br>" +
						tr("Artist:") + "</b> <b>" + mediaInfo.metadataValue(MediaInfo::Artist) + "</b><br>" +
						tr("Album:") + "</b> <b>" + mediaInfo.metadataValue(MediaInfo::Album) + "</b><br>" +
						tr("Length:") + "</b> <b>" + mediaInfo.length() + "</b>";
				} else {
					tmp = filename;

					//Resolve metadata file one by one
					if (_mediaInfoFetcherIndex == QModelIndex()) {
						_mediaInfoFetcherIndex = index;
						_mediaInfoFetcher->start(filename);
					}
				}
			} else {
				//If the filename is in fact a directory
				//then we don't try to resolve the meta data
				//since obviously there is none
				//If the filename is a file without a "multimedia" extension (i.e .mp3, .avi, .flac...)
				//then we don't try to resolve the meta data
				tmp = filename;
			}
			break;
		}
		break;
	}
	}

	return tmp;
}

QModelIndex FileSearchModel::index(int row, int column, const QModelIndex & parent) const {
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	FileSearchItem * parentItem = NULL;
	if (!parent.isValid()) {
		parentItem = _rootItem;
	} else {
		parentItem = static_cast<FileSearchItem *>(parent.internalPointer());
	}

	FileSearchItem * childItem = parentItem->child(row);
	if (childItem) {
		return createIndex(row, column, childItem);
	} else {
		return QModelIndex();
	}
}

QModelIndex FileSearchModel::parent(const QModelIndex & index) const {
	if (!index.isValid()) {
		return QModelIndex();
	}

	FileSearchItem * childItem = static_cast<FileSearchItem *>(index.internalPointer());
	FileSearchItem * parentItem = childItem->parent();

	if (parentItem == _rootItem) {
		return QModelIndex();
	}

	return createIndex(parentItem->row(), COLUMN_FILENAME, parentItem);
}

int FileSearchModel::rowCount(const QModelIndex & parent) const {
	FileSearchItem * parentItem = NULL;
	if (!parent.isValid()) {
		parentItem = _rootItem;
	} else {
		parentItem = static_cast<FileSearchItem *>(parent.internalPointer());
	}

	int childCount = 0;
	if (parentItem) {
		childCount = parentItem->childCount();
	}
	return childCount;
}

Qt::ItemFlags FileSearchModel::flags(const QModelIndex & index) const {
	if (!index.isValid()) {
		return 0;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

bool FileSearchModel::hasChildren(const QModelIndex & parent) const {
	if (parent.column() > COLUMN_FILENAME) {
		return false;
	}

	bool tmp = false;
	FileSearchItem * item = static_cast<FileSearchItem *>(parent.internalPointer());
	if (!item) {
		qWarning() << __FUNCTION__ << "Error: item is NULL";
	} else {
		//Optimization: QFileInfo::isDir() is too slow, replaced by TkFile::isDir()
		tmp = TkFile::isDir(item->mediaInfo().fileName());
	}

	return tmp;
}

bool FileSearchModel::canFetchMore(const QModelIndex & parent) const {
	FileSearchItem * item = static_cast<FileSearchItem *>(parent.internalPointer());
	if (item && hasChildren(parent)) {
		return !(item->populatedChildren());
	} else {
		return false;
	}
}

void FileSearchModel::fetchMore(const QModelIndex & parent) {
	_currentParentItem = static_cast<FileSearchItem *>(parent.internalPointer());

	//_currentParentQModelIndex is a hack because of beginInsertRows()
	_currentParentQModelIndex = parent;

	QString path = fileInfo(parent).absoluteFilePath();
	search(path, QRegExp(QString(), Qt::CaseInsensitive, QRegExp::RegExp2), INT_MAX, false);
}

QMimeData * FileSearchModel::mimeData(const QModelIndexList & indexes) const {
	QMimeData * mimeData = new QMimeData();
	QStringList files;
	foreach (QModelIndex index, indexes) {
		FileSearchItem * item = static_cast<FileSearchItem *>(index.internalPointer());
		if (!item) {
			qWarning() << __FUNCTION__ << "Error: item is NULL";
			continue;
		}
		const MediaInfo & mediaInfo(item->mediaInfo());
		QString filename(mediaInfo.fileName());
		if (!filename.isEmpty()) {
			files << filename;
		}
	}

	mimeData->setData("text/uri-list", files.join("\n").toUtf8());

	return mimeData;
}

QStringList FileSearchModel::mimeTypes() const {
	QStringList types;
	types << "text/uri-list";
	return types;
}

Qt::DropActions FileSearchModel::supportedDropActions() const {
	return Qt::CopyAction | Qt::MoveAction;
}

QFileInfo FileSearchModel::fileInfo(const QModelIndex & index) const {
	QFileInfo tmp;

	FileSearchItem * item = static_cast<FileSearchItem *>(index.internalPointer());
	if (!item) {
		qWarning() << __FUNCTION__ << "Error: item is NULL";
	} else {
		tmp = QFileInfo(item->mediaInfo().fileName());
	}
	return tmp;
}

void FileSearchModel::reset() {
	if (_rootItem) {
		//Deteles _rootItem + all its childs
		//so everything is deleted
		delete _rootItem;
		_rootItem = NULL;
	}

	//No need to delete _currentParentItem since "delete _rootItem" will do it
	_currentParentItem = NULL;
	_currentParentQModelIndex = QModelIndex();

	_mediaInfoFetcherIndex = QModelIndex();

	//Resets the model
	QAbstractItemModel::reset();
}

void FileSearchModel::search(const QString & path, const QRegExp & pattern, int filesFoundLimit, bool recursiveSearch) {
	qDebug() << __FUNCTION__ << path;

	if (!_currentParentItem) {
		if (!_rootItem) {
			//Lazy initialization of _rootItem
			_rootItem = new FileSearchItem(path, NULL);
		}
		_currentParentItem = _rootItem;
	}

	//Item is going to be populated, let's say it is already
	//because population of an item is threaded
	_currentParentItem->setPopulatedChildren(true);

	if (_findFiles) {
		//Uninitialize/remove/disconnect... previous _findFiles
		disconnect(_findFiles, SIGNAL(filesFound(const QStringList &)),
			this, SLOT(filesFound(const QStringList &)));
		disconnect(_findFiles, SIGNAL(finished(int)),
			this, SIGNAL(searchFinished(int)));
	}

	//Stops the previous search
	//Do it first (i.e before setPattern(), setExtensions()...) otherwise it can crash
	//inside FindFiles since there is no mutex
	stop();

	//Starts a new search
	delete _findFiles;
	_findFiles = new FindFiles(this);

	//Starts a new file search
	_findFiles->setSearchPath(path);

	//This was true with Qt 4.4.3
	//Way faster with INT_MAX because beginInsertRows() is slow
	//It's better to call only once thus INT_MAX instead of 1 for example
	//_findFiles->setFilesFoundLimit(INT_MAX);
	//Now with Qt 4.5.1, speed is really good :-)
	_findFiles->setFilesFoundLimit(filesFoundLimit);

	_findFiles->setPattern(pattern);
	_findFiles->setExtensions(_searchExtensions);
	_findFiles->setFindDirs(true);
	_findFiles->setRecursiveSearch(recursiveSearch);
	connect(_findFiles, SIGNAL(filesFound(const QStringList &)),
		SLOT(filesFound(const QStringList &)), Qt::QueuedConnection);
	connect(_findFiles, SIGNAL(finished(int)),
		SIGNAL(searchFinished(int)), Qt::QueuedConnection);
	_findFiles->start();
}

void FileSearchModel::stop() {
	if (_findFiles) {
		_findFiles->stop();
	}
}

void FileSearchModel::filesFound(const QStringList & files) {
	if (sender() != _findFiles) {
		//filesFound() signal from a previous _findFiles,
		//let's discards it
		return;
	}

	//Append the files
	int first = _currentParentItem->childCount();
	int last = first + files.size() - 1;

	beginInsertRows(_currentParentQModelIndex, first, last);
	foreach (QString filename, files) {
		_currentParentItem->appendChild(new FileSearchItem(filename, _currentParentItem));
	}
	endInsertRows();
}

void FileSearchModel::updateMediaInfo() {
	if (_mediaInfoFetcherIndex == QModelIndex() || !_mediaInfoFetcherIndex.isValid()) {
		qCritical() << __FUNCTION__ << "Error: _mediaInfoFetcherIndex invalid";
	} else {
		FileSearchItem * item = static_cast<FileSearchItem *>(_mediaInfoFetcherIndex.internalPointer());
		if (!item) {
			qWarning() << __FUNCTION__ << "Error: item is NULL";
			return;
		}

		MediaInfo mediaInfo = item->mediaInfo();

		if (mediaInfo.fileName() == _mediaInfoFetcher->mediaInfo().fileName() &&
			!mediaInfo.fetched()) {

			item->setMediaInfo(_mediaInfoFetcher->mediaInfo());

			//Update the index since the matching MediaSource has been modified
			emit dataChanged(_mediaInfoFetcherIndex, _mediaInfoFetcherIndex);
		}
	}
	_mediaInfoFetcherIndex = QModelIndex();
}
