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

#ifndef FILESEARCHITEM_H
#define FILESEARCHITEM_H

#include <QtCore/QList>
#include <QtCore/QVariant>

/**
 * A container for items of data supplied by the simple tree model.
 */
class FileSearchItem {
public:

	FileSearchItem(FileSearchItem * parent = 0);
	~FileSearchItem();

	void appendChild(FileSearchItem * child);

	FileSearchItem * child(int row);
	int childCount() const;
	int columnCount() const;
	QVariant data(int column) const;
	int row() const;
	FileSearchItem * parent();

private:

	QList<FileSearchItem *> _childItems;
	FileSearchItem * _parentItem;
};

#endif	//FILESEARCHITEM_H
