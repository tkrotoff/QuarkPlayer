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

#ifndef DIRMODEL_H
#define DIRMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>

/**
 * Minimal dir model that replaces QDirModel and QFileSystemModel.
 *
 * DirModel is read-only and threaded.
 *
 * DirModel was created since it is difficult to perform a recursive search
 * using QDirModel and QFileSystemModel.
 * DirModel was specially designed for the FileBrowser plugin of QuarkPlayer.
 *
 * @see QDirModel
 * @see QFileSystemModel
 * @author Tanguy Krotoff
 */
class DirModel : public QAbstractItemModel {
	Q_OBJECT
public:

	static const int COLUMN_FILENAME;

	DirModel(QObject * parent);

	~DirModel();

	void setRootPath(const QString & path);

	QFileInfo fileInfo(const QModelIndex & index) const;

	//Inherited from QAbstractItemModel
	int columnCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex & index) const;
	int rowCount(const QModelIndex & parent = QModelIndex()) const;
	bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

	Qt::ItemFlags flags(const QModelIndex & index) const;

	//bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);
	QMimeData * mimeData(const QModelIndexList & indexes) const;
	QStringList mimeTypes() const;
	Qt::DropActions supportedDropActions() const;

public slots:

private slots:

	void filesFound(const QStringList & files);

private:

	QString _rootPath;

	QStringList _filenames;
};

#endif	//DIRMODEL_H
