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

#include "FileSearchModel.h"

#include <mediainfowindow/MediaInfoFetcher.h>

#include <filetypes/FileTypes.h>

#include <tkutil/FindFiles.h>
#include <tkutil/TkFile.h>

#include <QtGui/QtGui>

#include <QtCore/QtConcurrentRun>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include <climits>

const int FileSearchModel::COLUMN_FILENAME = 0;
const int FileSearchModel::COLUMN_FIRST = COLUMN_FILENAME;
const int FileSearchModel::COLUMN_LAST = COLUMN_FILENAME;

static const int COLUMN_COUNT = 1;

static const int POSITION_INVALID = -1;

QHash<QString, QIcon> FileSearchModel::_iconsCache;

FileSearchModel::FileSearchModel(QObject * parent)
	: QAbstractItemModel(parent) {

	_findFiles = NULL;

	clearInternal();

	//Info fetcher
	_mediaInfoFetcher = new MediaInfoFetcher(this);
	connect(_mediaInfoFetcher, SIGNAL(fetched()), SLOT(updateMediaInfo()));
}

FileSearchModel::~FileSearchModel() {
}

int FileSearchModel::columnCount(const QModelIndex & parent) const {
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

	MediaInfo mediaInfo = _filenames[row];
	QString filename(mediaInfo.fileName());

	switch (role) {
	case Qt::DisplayRole: {
		switch (column) {
		case COLUMN_FILENAME:
			tmp = TkFile::fileName(filename);
			break;
		}
		break;
	}

	case Qt::DecorationRole: {
		switch (column) {
		case COLUMN_FILENAME:
			if (_iconProvider) {
				//This is too slow:
				//re-creates an icon for each file
				//We want a cache system -> way faster!
				//tmp = _iconProvider->icon(QFileInfo(filename));

				QString ext(TkFile::fileExtension(filename));
				if (!_iconsCache.contains(ext)) {
					_iconsCache[ext] = _iconProvider->icon(QFileInfo(filename));
				}
				tmp = _iconsCache.value(ext);
			}
			break;
		}
		break;
	}

	case Qt::ToolTipRole: {
		switch (column) {
		case COLUMN_FILENAME:
			if (!TkFile::isDir(filename)) {
				if (mediaInfo.fetched()) {
					tmp = filename + "<br>" +
						tr("Title:") + "</b> <b>" + mediaInfo.metadataValue(MediaInfo::Title) + "</b><br>" +
						tr("Artist:") + "</b> <b>" + mediaInfo.metadataValue(MediaInfo::Artist) + "</b><br>" +
						tr("Album:") + "</b> <b>" + mediaInfo.metadataValue(MediaInfo::Album) + "</b><br>" +
						tr("Length:") + "</b> <b>" + mediaInfo.length() + "</b>";
				} else {
					tmp = filename;

					//Resolve metadata file one by one
					if (_mediaInfoFetcherRow == POSITION_INVALID) {
						_mediaInfoFetcherRow = row;
						_mediaInfoFetcher->start(filename);
					}
				}
			} else {
				//If the filename is in fact a directory
				//then we don't try to resolve the meta data
				//since obviously there is none
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
	if (parent.isValid()) {
		return QModelIndex();
	}

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	return createIndex(row, column);
}

QModelIndex FileSearchModel::parent(const QModelIndex & index) const {
	return QModelIndex();
}

int FileSearchModel::rowCount(const QModelIndex & parent) const {
	if (parent.isValid()) {
		return 0;
	}

	return _filenames.size();
}

Qt::ItemFlags FileSearchModel::flags(const QModelIndex & index) const {
	if (!index.isValid()) {
		return Qt::ItemIsDropEnabled;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

QMimeData * FileSearchModel::mimeData(const QModelIndexList & indexes) const {
	QMimeData * mimeData = new QMimeData();
	QStringList files;
	foreach (QModelIndex index, indexes) {
		//Number of index is the number of columns
		//Here we have 5 columns, we only want to select 1 column per row
		if (index.column() == COLUMN_FILENAME) {
			int dragAndDropRow = index.row();
			QString filename(_filenames[dragAndDropRow].fileName());
			if (!filename.isEmpty()) {
				files << filename;
			}
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
	if (index.isValid()) {
		int row = index.row();
		tmp = QFileInfo(_filenames[row].fileName());
	}
	return tmp;
}

void FileSearchModel::setIconProvider(QFileIconProvider * provider) {
	_iconProvider = provider;
}

void FileSearchModel::clearInternal() {
	_filenames.clear();
	_mediaInfoFetcherRow = POSITION_INVALID;
}

void FileSearchModel::search(const QString & path, const QRegExp & pattern, const QStringList & extensions) {
	qDebug() << __FUNCTION__ << path;

	//Uninitialize/remove/disconnect... previous _findFiles
	disconnect(_findFiles, SIGNAL(filesFound(const QStringList &)), this,
		SLOT(filesFound(const QStringList &)));
	disconnect(_findFiles, SIGNAL(finished(int)), this,
		SIGNAL(searchFinished(int)));

	//Stops the previous search
	//Do it first (i.e before setPattern(), setExtensions()...) otherwise it can crash
	//inside FindFiles since there is no mutex
	stop();

	//Clears the model before starting a new search
	clearInternal();
	reset();

	//Starts a new search
	delete _findFiles;
	_findFiles = new FindFiles(this);

	//Starts a new file search
	_findFiles->setSearchPath(path);

	//Way faster with INT_MAX because beginInsertRows() is slow
	//It's better to call only once thus INT_MAX instead of 1 for example
	_findFiles->setFilesFoundLimit(INT_MAX);

	_findFiles->setPattern(pattern);
	_findFiles->setExtensions(extensions);
	_findFiles->setFindDirs(true);
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
		//filesFound() signal from a previous _findFiles
		return;
	}

	//Append the files
	int first = _filenames.size();
	int last = first + files.size() - 1;
	int currentRow = first;

	beginInsertRows(QModelIndex(), first, last);
	foreach (QString filename, files) {
		_filenames.insert(currentRow, MediaInfo(filename));
		currentRow++;
	}
	endInsertRows();
}

void FileSearchModel::updateMediaInfo() {
	if (_mediaInfoFetcherRow == POSITION_INVALID) {
		qCritical() << __FUNCTION__ << "Error: _mediaInfoFetcherRow invalid";
	} else {
		if (_mediaInfoFetcherRow < _filenames.size()) {
			MediaInfo mediaInfo = _filenames[_mediaInfoFetcherRow];

			if (mediaInfo.fileName() == _mediaInfoFetcher->mediaInfo().fileName() &&
				!mediaInfo.fetched()) {

				_filenames[_mediaInfoFetcherRow] = _mediaInfoFetcher->mediaInfo();

				//Update the row since the matching MediaSource has been modified
				emit dataChanged(index(_mediaInfoFetcherRow, COLUMN_FIRST), index(_mediaInfoFetcherRow, COLUMN_LAST));
			}
		}
	}
	_mediaInfoFetcherRow = POSITION_INVALID;
}
