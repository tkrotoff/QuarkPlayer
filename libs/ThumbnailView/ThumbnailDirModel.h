/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2004-2007  Wengo
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef THUMBNAILDIRMODEL_H
#define THUMBNAILDIRMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QDir>
#include <QtCore/QStringList>

class ThumbnailManager;

/**
 * A Qt model, similar to QDirModel but which does not store the whole file
 * tree and does not require to go through a QSortFilterProxyModel to be
 * filtered.
 *
 * Using QDirModel and QSortFilterProxyModel causes troubles because
 * QSortFilterProxyModel will filter all parents of the view root index on
 * first call. This operation can be slow if the filter is CPU expensive.
 *
 * @author Aur�lien G�teau
 * @author Tanguy Krotoff
 */
class ThumbnailDirModel : public QAbstractListModel {
	Q_OBJECT
public:

	ThumbnailDirModel(QObject * parent);

	/**
	 * Sets the thumbnails size.
	 */
	void setThumbnailSize(int size);

	/**
	 * Sets the dir to list.
	 */
	void setDir(const QString & dir);

	int rowCount(const QModelIndex & parent = QModelIndex()) const;

	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

public slots:

	void refresh();

private slots:

	void pixmapUpdated(const QFileInfo & updatedFileInfo);

private:

	ThumbnailManager * _thumbnailManager;

	QDir _dir;

	QFileInfoList _infoList;

	QStringList _imageSuffixList;
};

#endif	//THUMBNAILDIRMODEL_H
