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

class QFileIconProvider;
class QFileInfo;
class QRegExp;

/**
 * Replacement for QDirModel or QFileSystemModel,
 * simple flat model populated via a recursive file search.
 *
 * Based on FindFiles
 *
 * @see QDirModel
 * @see QFileSystemModel
 * @see FindFiles
 * @author Tanguy Krotoff
 */
class FileSearchModel : public QAbstractItemModel {
	Q_OBJECT
public:

	static const int COLUMN_FILENAME;
	static const int COLUMN_PATH;

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

	//Inherited from QAbstractItemModel
	int columnCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex & index) const;
	int rowCount(const QModelIndex & parent = QModelIndex()) const;
	bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

	Qt::ItemFlags flags(const QModelIndex & index) const;

	QMimeData * mimeData(const QModelIndexList & indexes) const;
	QStringList mimeTypes() const;
	Qt::DropActions supportedDropActions() const;

private slots:

	void filesFound(const QStringList & files);

private:

	QFileIconProvider * _iconProvider;

	QStringList _filenames;
};

#endif	//FILESEARCHMODEL_H
