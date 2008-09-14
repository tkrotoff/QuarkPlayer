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

const int FileSearchModel::COLUMN_FILENAME = 0;

static const int COLUMN_COUNT = 1;

FileSearchModel::FileSearchModel(QObject * parent)
	: QAbstractItemModel(parent) {

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

	switch (role) {
	case Qt::DisplayRole: {
		switch (column) {
		case COLUMN_FILENAME:
			tmp = TkFile::fileName(_filenames[row]);
			break;
		default:
			qCritical() << __FUNCTION__ << "Error: unknown column:" << column;
		}
		break;
	}

	/*
	case Qt::DecorationRole: {
		switch (column) {
		case COLUMN_FILENAME: {
			QIcon icon = icon(index);
			if (icon.isNull()) {
				if (node(index)->isDir()) {
					icon = d->fileInfoGatherer.iconProvider()->icon(QFileIconProvider::Folder);
				} else {
					icon = d->fileInfoGatherer.iconProvider()->icon(QFileIconProvider::File);
				}
			}
			tmp = icon;
			break;
		}
		default:
			qCritical() << __FUNCTION__ << "Error: unknown column:" << column;
		}
		break;
	}
	*/

	/*default:
		qCritical() << __FUNCTION__ << "Error: unknown role:" << role;*/
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

bool FileSearchModel::removeRows(int row, int count, const QModelIndex & parent) {
	beginRemoveRows(QModelIndex(), row, row + count - 1);
	for (int i = 0; i < count; i++) {
		qDebug() << __FUNCTION__ << "Remove row:" << row;

		//Remove from the list of filenames
		_filenames.removeAt(row);
	}
	endRemoveRows();

	return true;
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

void FileSearchModel::search(const QString & path, const QString & pattern, const QStringList & extensions) {
	qDebug() << __FUNCTION__ << path;

	_rootPath = path;
	_pattern = pattern;
	_extensions = extensions;

	FindFiles findFiles;
	connect(&findFiles, SIGNAL(filesFound(const QStringList &)),
		SLOT(filesFound(const QStringList &)));
	findFiles.setSearchPath(_rootPath);
	findFiles.findAllFiles();
}

void FileSearchModel::filesFound(const QStringList & files) {
	qDebug() << __FUNCTION__ << files[0];

	QStringList filenameList;
	foreach (QString filename, files) {
		if (_extensions.contains(TkFile::fileExtension(filename), Qt::CaseInsensitive) &&
			filename.contains(_pattern, Qt::CaseInsensitive)) {

			filenameList << filename;
		}
	}

	if (!filenameList.isEmpty()) {
		//Append the files
		int first = _filenames.size();
		int last = first + filenameList.size() - 1;
		int currentRow = first;

		beginInsertRows(QModelIndex(), first, last);
		foreach (QString filename, filenameList) {
			_filenames.insert(currentRow, filename);
			currentRow++;
		}
		endInsertRows();

		QCoreApplication::processEvents();
	}
}
