/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "FileSearchItem.h"

#include <QtCore/QStringList>

FileSearchItem::FileSearchItem(FileSearchItem * parent) {
	_parentItem = parent;
}

FileSearchItem::~FileSearchItem() {
	qDeleteAll(_childItems);
}

void FileSearchItem::appendChild(FileSearchItem * item) {
	_childItems.append(item);
}

FileSearchItem * FileSearchItem::child(int row) {
	return _childItems.value(row);
}

int FileSearchItem::childCount() const {
	return _childItems.count();
}

int FileSearchItem::columnCount() const {
	return 1;
}

QVariant FileSearchItem::data(int column) const {
	return itemData.value(column);
}

FileSearchItem * FileSearchItem::parent() {
	return _parentItem;
}

int FileSearchItem::row() const {
	if (_parentItem) {
		return _parentItem->_childItems.indexOf(const_cast<FileSearchItem *>(this));
	}

	return 0;
}
