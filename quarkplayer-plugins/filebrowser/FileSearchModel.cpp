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

#include <filetypes/FileTypes.h>

#include <tkutil/FindFiles.h>
#include <tkutil/TkFile.h>

#include <QtGui/QtGui>

#include <QtCore/QtConcurrentRun>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include <climits>

const int FileSearchModel::COLUMN_FILENAME = 0;
const int FileSearchModel::COLUMN_PATH = 1;

static const int COLUMN_COUNT = 2;

QMap<QString, QIcon> FileSearchModel::_iconsCache;

FileSearchModel::FileSearchModel(QObject * parent)
	: QAbstractItemModel(parent) {

	_findFiles = new FindFiles(this);
	connect(_findFiles, SIGNAL(filesFound(const QStringList &)),
		SLOT(filesFound(const QStringList &)), Qt::QueuedConnection);
	connect(_findFiles, SIGNAL(finished(int)),
		SIGNAL(searchFinished(int)), Qt::QueuedConnection);
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
		case COLUMN_PATH:
			tmp = tr("Path");
			break;
		default:
			qCritical() << __FUNCTION__ << "Error: unknown column:" << section;
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

	QString filename(_filenames[row]);

	switch (role) {
	case Qt::DisplayRole: {
		switch (column) {
		case COLUMN_FILENAME:
			tmp = TkFile::fileName(filename);
			break;
		case COLUMN_PATH:
			tmp = TkFile::path(filename);
			break;
		default:
			qCritical() << __FUNCTION__ << "Error: unknown column:" << column;
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
		case COLUMN_PATH:
			break;
		default:
			qCritical() << __FUNCTION__ << "Error: unknown column:" << column;
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
			QString filename(_filenames[index.row()]);
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
		tmp = QFileInfo(_filenames[index.row()]);
	}

	return tmp;
}

void FileSearchModel::setIconProvider(QFileIconProvider * provider) {
	_iconProvider = provider;
}

void FileSearchModel::search(const QString & path, const QRegExp & pattern, const QStringList & extensions) {
	qDebug() << __FUNCTION__ << path;

	//Clears the model before starting a new search
	_filenames.clear();
	reset();

	//Starts a new file search
	_findFiles->setSearchPath(path);

	//Way faster with INT_MAX
	_findFiles->setFilesFoundLimit(INT_MAX);

	_findFiles->setPattern(pattern);
	_findFiles->setExtensions(extensions);
	_findFiles->setFindDirs(true);
	_findFiles->stop();
	_findFiles->start();
}

void FileSearchModel::stop() {
	_findFiles->stop();
}

void FileSearchModel::filesFound(const QStringList & files) {
	//Append the files
	int first = _filenames.size();
	int last = first + files.size() - 1;
	int currentRow = first;

	beginInsertRows(QModelIndex(), first, last);
	foreach (QString filename, files) {
		_filenames.insert(currentRow, filename);
		currentRow++;
	}
	endInsertRows();
}
