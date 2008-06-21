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

#ifndef SIMPLEDIRMODEL_H
#define SIMPLEDIRMODEL_H

#include <QtGui/QDirModel>

/**
 * Simple and fast QDirModel.
 *
 * @see QDirModel
 * @author Tanguy Krotoff
 */
class SimpleDirModel : public QDirModel {
public:

	SimpleDirModel(const QStringList & nameFilters);

	~SimpleDirModel();

	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;

	QModelIndex index(const QString & path, int column = 0) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	int columnCount(const QModelIndex & parent) const;

private:

};

#endif	//SIMPLEDIRMODEL_H
