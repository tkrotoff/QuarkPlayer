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

#ifndef FILEBROWSERFILTER_H
#define FILEBROWSERFILTER_H

#include <QtGui/QSortFilterProxyModel>

class FileBrowserTreeView;

/**
 * Sorts/filters the file browser tree view given a string.
 *
 * @author Tanguy Krotoff
 */
class FileBrowserFilter : public QSortFilterProxyModel {
	Q_OBJECT
public:

	FileBrowserFilter(QObject * parent, FileBrowserTreeView * treeView);

	void setFilter(const QString & filter);

	/** This is important, otherwise the tree view only shows root items. */
	bool hasChildren(const QModelIndex & parent = QModelIndex()) const;

private:

	bool filterAcceptsColumn(int sourceColumn, const QModelIndex & sourceParent) const;

	bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const;

	bool indexMatches(const QModelIndex & index) const;

	QModelIndexList findAllParents(const QModelIndex & index) const;

	QModelIndexList findAllChilds(const QModelIndex & index) const;

	QString _filter;

	FileBrowserTreeView * _treeView;
};

#endif	//FILEBROWSERFILTER_H
