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

#include "FastDirModel.h"

#include <QtGui/QtGui>

FastDirModel::FastDirModel(const QStringList & nameFilters)
	: QFileSystemModel() {

	setNameFilters(nameFilters);
	setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
	//setSorting(QDir::Name | QDir::DirsFirst);
	setReadOnly(true);
	setNameFilterDisables(false);
}

FastDirModel::~FastDirModel() {
}

QModelIndex FastDirModel::index(int row, int column, const QModelIndex & parent) const {
	switch (column) {
	case 0:
		return QFileSystemModel::index(row, column, parent);
	default:
		return QModelIndex();
	}
}

QModelIndex FastDirModel::index(const QString & path, int column) const {
	switch (column) {
	case 0:
		return QFileSystemModel::index(path, column);
	default:
		return QModelIndex();
	}
}

QVariant FastDirModel::headerData(int section, Qt::Orientation orientation, int role) const {
	switch (section) {
	case 0:
		return QFileSystemModel::headerData(section, orientation, role);
	default:
		return QAbstractItemModel::headerData(section, orientation, role);
	}
}

QVariant FastDirModel::data(const QModelIndex & index, int role) const {
	switch (index.column()) {
	case 0:
		return QFileSystemModel::data(index, role);
	default:
		return QVariant();
	}
}

int FastDirModel::columnCount(const QModelIndex & parent) const {
	return 1;
}

void FastDirModel::refresh() {
}
