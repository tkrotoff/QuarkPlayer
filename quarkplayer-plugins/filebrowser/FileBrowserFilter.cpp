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

#include "FileBrowserFilter.h"

#include "FileBrowserTreeView.h"

#include <QtCore/QStringList>
#include <QtCore/QDebug>

FileBrowserFilter::FileBrowserFilter(QObject * parent, FileBrowserTreeView * treeView)
	: QSortFilterProxyModel(parent) {

	_treeView = treeView;
}

bool FileBrowserFilter::hasChildren(const QModelIndex & parent) const {
	return sourceModel()->hasChildren(mapToSource(parent));
}

bool FileBrowserFilter::filterAcceptsColumn(int sourceColumn, const QModelIndex & sourceParent) const {
	return true;
}

bool FileBrowserFilter::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const {
	if (_filter.isEmpty()) {
		return true;
	}

	QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

	bool keep = false;
	if (indexMatches(index)) {
		//If the item directly matches, then exit
		//qDebug() << __FUNCTION__ << "index matching";
		keep = true;
	} else {
		//qDebug() << __FUNCTION__ << "NO MATCH";
		//The item does not match directly

		//Let's check if a parent matches, then
		//the item itself matches, if it is the case, we exit the loop
		QModelIndexList parents = findAllParents(index);
		foreach (QModelIndex parent, parents) {
			if (indexMatches(parent)) {
				//qDebug() << __FUNCTION__ << "parent matching";
				keep = true;
				break;
			}
		}

		if (!keep) {
			QString filename(sourceModel()->data(index).toString());
			qDebug() << "papa:" << filename << index.row() << index.column();

			//No parent matches, let's check the childs
			//The first one that matches, then we exit the loop
			QModelIndexList childs = findAllChilds(index);
			foreach (QModelIndex child, childs) {
				if (indexMatches(child)) {
					qDebug() << __FUNCTION__ << "child matching";
					keep = true;
					break;
				}
			}
		}
	}

	return keep;
}

void FileBrowserFilter::setFilter(const QString & filter) {
	if (filter != _filter) {
		_filter = filter;
		invalidateFilter();
	}
}

bool FileBrowserFilter::indexMatches(const QModelIndex & index) const {
	QString filename(sourceModel()->data(index).toString());
	bool keep = false;
	foreach(QString word, _filter.split(' ')) {
		keep = filename.contains(word, Qt::CaseInsensitive);
		if (keep) {
			qDebug() << __FUNCTION__ << "Match:" << filename;
			break;
		}
	}
	return keep;
}

QModelIndexList FileBrowserFilter::findAllParents(const QModelIndex & index) const {
	QModelIndexList indexList;

	if (index.isValid()) {
		QModelIndex parent = index.parent();
		if (parent.isValid()) {
			indexList += parent;
			indexList += findAllParents(parent);
		}
	}

	return indexList;
}

QModelIndexList FileBrowserFilter::findAllChilds(const QModelIndex & index) const {
	QModelIndexList indexList;

	//_treeView->expand(mapFromSource(index));

	if (index.isValid()) {
		int row = 0;
		int column = index.column();
		indexList += index;

		QString filename(sourceModel()->data(index).toString());
		qDebug() << __FUNCTION__ << filename << index.row() << index.column();

		QModelIndex child;
		do {
			child = sourceModel()->index(row++, column, index);
			//child = index.child(row++, column);

			if (child.isValid()) {
				QString filename(sourceModel()->data(child).toString());
				qDebug() << __FUNCTION__ << filename << child.row() << child.column();

				indexList += findAllChilds(child);
			}

		} while(child.isValid());
	}

	return indexList;
}

/*
QModelIndexList FileBrowserFilter::findAllChilds(const QModelIndex & index) const {
	QModelIndexList indexList;

	_treeView->setExpanded(index, true);

	int rows = sourceModel()->rowCount(index);

	for (int row = 0; row < rows; row++) {
		QModelIndex child = sourceModel()->index(row, 0, index);//index.child(row, index.column());
		qDebug() << row << sourceModel()->data(child, Qt::DisplayRole);
		if (child.isValid()) {
			findAllChilds(child);
		}
	}

	return indexList;
}
*/

/*
QModelIndexList FileBrowserFilter::findAllChilds(const QModelIndex & index) const {
	QModelIndexList match = sourceModel()->match(index, Qt::DisplayRole, _filter);
	foreach (QModelIndex index, match) {
		qDebug() << __FUNCTION__ << sourceModel()->data(index, Qt::DisplayRole);
	}
	return match;
}
*/
