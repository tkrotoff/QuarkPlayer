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

#include "DirModel.h"

#include <filetypes/FileTypes.h>

#include <tkutil/FindFiles.h>
#include <tkutil/TkFile.h>

#include <QtGui/QtGui>

#include <QtCore/QtConcurrentRun>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

const int DirModel::COLUMN_FILENAME = 0;

static const int COLUMN_COUNT = 1;

DirModel::DirModel(QObject * parent)
	: QAbstractItemModel(parent) {

}

DirModel::~DirModel() {
}

int DirModel::columnCount(const QModelIndex & parent) const {
	return COLUMN_COUNT;
}

QVariant DirModel::headerData(int section, Qt::Orientation orientation, int role) const {
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

QVariant DirModel::data(const QModelIndex & index, int role) const {
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
			tmp = _filenames[row];
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

QModelIndex DirModel::index(int row, int column, const QModelIndex & parent) const {
	if (parent.isValid()) {
		return QModelIndex();
	}

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	return createIndex(row, column);
}

QModelIndex DirModel::parent(const QModelIndex & index) const {
	return QModelIndex();
}

int DirModel::rowCount(const QModelIndex & parent) const {
	if (parent.isValid()) {
		return 0;
	}

	return _filenames.size();
}

Qt::ItemFlags DirModel::flags(const QModelIndex & index) const {
	if (!index.isValid()) {
		return Qt::ItemIsDropEnabled;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

bool DirModel::removeRows(int row, int count, const QModelIndex & parent) {
	beginRemoveRows(QModelIndex(), row, row + count - 1);
	for (int i = 0; i < count; i++) {
		qDebug() << __FUNCTION__ << "Remove row:" << row;

		//Remove from the list of filenames
		_filenames.removeAt(row);
	}
	endRemoveRows();

	return true;
}

QMimeData * DirModel::mimeData(const QModelIndexList & indexes) const {
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

QStringList DirModel::mimeTypes() const {
	QStringList types;
	types << "text/uri-list";
	return types;
}

Qt::DropActions DirModel::supportedDropActions() const {
	return Qt::CopyAction | Qt::MoveAction;
}

void DirModel::setRootPath(const QString & path) {
	qDebug() << __FUNCTION__ << path;

	_rootPath = path;

	FindFiles findFiles;
	connect(&findFiles, SIGNAL(filesFound(const QStringList &)),
		SLOT(filesFound(const QStringList &)));
	findFiles.setSearchPath(_rootPath);
	findFiles.findAllFiles();
}

void DirModel::filesFound(const QStringList & files) {
	qDebug() << __FUNCTION__ << files[0];

	if (!files.isEmpty()) {
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

		QCoreApplication::processEvents();
	}
}

QFileInfo DirModel::fileInfo(const QModelIndex & index) const {
	QFileInfo fileInfo;

	if (index.isValid()) {
		fileInfo = QFileInfo(_filenames[index.row()]);
	}

	return fileInfo;
}
