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

#include "FileSearchModel.h"

#include "FileSearchItem.h"
#include "FileBrowserLogger.h"

#include <MediaInfoFetcher/MediaInfoFetcher.h>

#include <FileTypes/FileTypes.h>

#include <TkUtil/FindFiles.h>
#include <TkUtil/TkFile.h>

#include <quarkplayer/config/Config.h>

#include <QtGui/QtGui>

#include <QtCore/QCoreApplication>

//For INT_MAX
#include <climits>

static const int COLUMN_FILENAME = 0;
static const int COLUMN_COUNT = COLUMN_FILENAME + 1;

QHash<QString, QIcon> FileSearchModel::_iconsCache;

FileSearchModel::FileSearchModel(QObject * parent)
	: QAbstractItemModel(parent) {

	_rootItem = NULL;
	_currentParentItem = NULL;

	reset();

	_findFiles = NULL;

	//Info fetcher
	_mediaInfoFetcher = new MediaInfoFetcher(this);
	connect(_mediaInfoFetcher, SIGNAL(finished(const MediaInfo &)),
		SLOT(updateMediaInfo(const MediaInfo &)));
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

void FileSearchModel::setRootSearchPath(const QString & rootSearchPath) {
	_rootSearchPath = rootSearchPath;
}

FileSearchItem * FileSearchModel::item(const QModelIndex & index) const {
	return static_cast<FileSearchItem *>(index.internalPointer());
}

QModelIndex FileSearchModel::index(const FileSearchItem * item) const {
	Q_ASSERT(item);

	FileSearchItem * parentItem = item->parent();
	int row = parentItem->childRow(item->fileName());
	return createIndex(row, COLUMN_FILENAME, const_cast<FileSearchItem *>(item));
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
		default:
			FileBrowserCritical() << "Unknown section:" << section;
		}
	}

	return tmp;
}

QVariant FileSearchModel::data(const QModelIndex & index, int role) const {
	QVariant tmp;

	if (!index.isValid() || index.model() != this) {
		return tmp;
	}

	int column = index.column();

	const MediaInfo & mediaInfo(item(index)->mediaInfo());
	QString fileName(mediaInfo.fileName());

	switch (role) {
	case Qt::DisplayRole: {
		switch (column) {
		case COLUMN_FILENAME:
			tmp = QFileInfo(fileName).fileName();
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
			//tmp = _iconProvider.icon(QFileInfo(fileName));

			QFileInfo fileInfo(fileName);
			QString ext(fileInfo.suffix());
			/*
			Do not cache the icon matching a directory
			since it is not always the same icon under Mac OS X
			and Windows 7
			if (fileInfo.isDir()) {
				ext = "Directory";
			}*/
			if (ext.isEmpty()) {
				ext = fileName;
			}
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
			//Make the filename relative to the search path
			//This way the tooltip is more readable because it is shorter
			QString relativeFileName(TkFile::relativeFilePath(_rootSearchPath, fileName));

			if (!QFileInfo(fileName).isDir() && FileTypes::fileExtensionMatches(fileName, _toolTipExtensions)) {
				if (mediaInfo.fetched()) {
					QString bitrate;
					if (mediaInfo.bitrate() > 0) {
						bitrate = QString("%1 %2 %3")
								.arg(mediaInfo.bitrate())
								.arg(tr("kbps"))
								.arg(mediaInfo.audioStreamValue(0, MediaInfo::AudioBitrateMode).toString());
					}
					tmp = relativeFileName + "<br>" +
						tr("Title:") + " <b>" + mediaInfo.metaDataValue(MediaInfo::Title).toString() + "</b><br>" +
						tr("Artist:") + " <b>" + mediaInfo.metaDataValue(MediaInfo::Artist).toString() + "</b><br>" +
						tr("Album:") + " <b>" + mediaInfo.metaDataValue(MediaInfo::Album).toString() + "</b><br>" +
						tr("Length:") + " <b>" + mediaInfo.durationFormatted() + "</b><br>" +
						tr("Bitrate:") + " <b>" + bitrate + "</b>";
				} else {
					tmp = relativeFileName;

					//Resolve metadata file one by one
					if (_mediaInfoFetcherIndex == QModelIndex()) {
						_mediaInfoFetcherIndex = index;
						_mediaInfoFetcher->start(MediaInfo(fileName));
					}
				}
			} else {
				//If the filename is in fact a directory
				//then we don't try to resolve the meta data
				//since obviously there is none
				//If the filename is a file without a "multimedia" extension (i.e .mp3, .avi, .flac...)
				//then we don't try to resolve the meta data
				tmp = relativeFileName;
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
		parentItem = item(parent);
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

	FileSearchItem * childItem = item(index);
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
		parentItem = item(parent);
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
	FileSearchItem * parentItem = NULL;
	if (!parent.isValid()) {
		parentItem = _rootItem;
	} else {
		parentItem = item(parent);
	}

	bool tmp = false;
	if (parentItem) {
		//Optimization: QFileInfo::isDir() is too slow, replaced by TkFile::isDir()
		bool isDir = parentItem->isDir();
		bool populating = parentItem->populatingChildren();
		bool childCount = parentItem->childCount() > 0;
		if (isDir && childCount) {
			tmp = true;
		}
		if (isDir && !populating) {
			tmp = true;
		}
	}

	return tmp;
}

bool FileSearchModel::canFetchMore(const QModelIndex & parent) const {
	if (hasChildren(parent)) {
		FileSearchItem * parentItem = NULL;
		if (!parent.isValid()) {
			parentItem = _rootItem;
		} else {
			parentItem = item(parent);
		}

		return !parentItem->populatingChildren();
	} else {
		return false;
	}
}

void FileSearchModel::fetchMore(const QModelIndex & parent) {
	if (!parent.isValid()) {
		return;
	}

	_currentParentItem = item(parent);

	QString path(fileInfo(parent).absoluteFilePath());
	search(path, QRegExp(QString(), Qt::CaseInsensitive, QRegExp::RegExp2), INT_MAX, false);
}

QMimeData * FileSearchModel::mimeData(const QModelIndexList & indexes) const {
	QMimeData * mimeData = new QMimeData();
	QStringList files;
	foreach (QModelIndex index, indexes) {
		const MediaInfo & mediaInfo(item(index)->mediaInfo());
		QString fileName(mediaInfo.fileName());
		if (!fileName.isEmpty()) {
			files << fileName;
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
	return QFileInfo(item(index)->fileName());
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

	_mediaInfoFetcherIndex = QModelIndex();

	//Resets the model
	QAbstractItemModel::reset();
}

void FileSearchModel::search(const QString & path, const QRegExp & pattern, int filesFoundLimit, bool recursiveSearch) {
	FileBrowserDebug() << path;

	if (!_currentParentItem) {
		if (!_rootItem) {
			//Lazy initialization of _rootItem
			_rootItem = new FileSearchItem(path, NULL);
		}
		_currentParentItem = _rootItem;
	}

	//Item is going to be populated via a separated thread
	_currentParentItem->setPopulatingChildren(true);

	//Stops the previous search if any
	//Do it first (i.e before setPattern(), setExtensions()...) otherwise it can crash
	//inside FindFiles since there is no mutex
	stop();

	if (!_findFiles) {
		//Lazy initialization
		_findFiles = new FindFiles(this);

		connect(_findFiles, SIGNAL(filesFound(const QStringList &)),
			SLOT(filesFound(const QStringList &)), Qt::QueuedConnection);
		connect(_findFiles, SIGNAL(finished(int)),
			SIGNAL(searchFinished(int)), Qt::QueuedConnection);
	}

	//This was true with Qt 4.4.3:
	//Way faster with INT_MAX because beginInsertRows() is slow
	//It's better to call only once thus INT_MAX instead of 1 for example
	//_findFiles->setFilesFoundLimit(INT_MAX);
	//Now with Qt 4.5.1, speed is really good :-)
	//_findFiles->setFilesFoundLimit(1);
	_findFiles->setFilesFoundLimit(filesFoundLimit);
	///

	_findFiles->setSearchPath(path);
	_findFiles->setPattern(pattern);
	_findFiles->setExtensions(_searchExtensions);
	_findFiles->setFindDirs(true);
	_findFiles->setRecursiveSearch(recursiveSearch);
	FindFiles::setBackend(static_cast<FindFiles::Backend>(
		Config::instance().value(Config::FINDFILES_BACKEND_KEY).toInt()));

	//Starts a new search
	_findFiles->start();
}

void FileSearchModel::stop() {
	if (_findFiles) {
		_findFiles->stop();
	}
}

void FileSearchModel::filesFound(const QStringList & files) {
	emit layoutAboutToBeChanged();

	//Append the files
	foreach (QString fileName, files) {
		/* Later filenames comparisons can fail if we don't convert / or \ */
		fileName = QDir::toNativeSeparators(fileName);
		_currentParentItem->appendChild(new FileSearchItem(fileName, _currentParentItem));
	}

	//Sort them
	_currentParentItem->sort();

	emit layoutChanged();
}

void FileSearchModel::updateMediaInfo(const MediaInfo & mediaInfo) {
	if (_mediaInfoFetcherIndex == QModelIndex() || !_mediaInfoFetcherIndex.isValid()) {
		FileBrowserCritical() << "_mediaInfoFetcherIndex invalid";
	} else {
		FileSearchItem * searchItem = item(_mediaInfoFetcherIndex);

		MediaInfo mediaInfo2 = searchItem->mediaInfo();

		if (mediaInfo2.fileName() == mediaInfo.fileName() &&
			!mediaInfo2.fetched()) {

			searchItem->setMediaInfo(mediaInfo);

			//Update the index since the matching MediaSource has been modified
			emit dataChanged(_mediaInfoFetcherIndex, _mediaInfoFetcherIndex);
		}
	}
	_mediaInfoFetcherIndex = QModelIndex();
}
