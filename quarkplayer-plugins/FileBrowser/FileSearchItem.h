/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILESEARCHITEM_H
#define FILESEARCHITEM_H

#include <MediaInfoFetcher/MediaInfo.h>

#include <QtCore/QList>

/**
 * A container for items of data supplied by the simple tree model.
 *
 * Inspired by Qt example "simpletreemodel".
 *
 * @author Tanguy Krotoff
 */
class FileSearchItem {
public:

	FileSearchItem(const QString & filename, FileSearchItem * parent);
	~FileSearchItem();

	/**
	 * This item is being populated.
	 *
	 * The item is being populated via a separated thread from class FindFiles.
	 *
	 * @see FindFiles
	 * @param populatingChildren true if this item is being populated; false otherwise
	 */
	void setPopulatingChildren(bool populatingChildren);
	bool populatingChildren() const;

	/**
	 * Appends an item to this item.
	 */
	void appendChild(FileSearchItem * newItem);

	/**
	 * Sorts the children of this item if any.
	 *
	 * Uses qStableSort() function from Qt.
	 * The sort is not recursive, there is no need.
	 */
	void sort();

	/** Gets the item child (if any) at the given row index. */
	FileSearchItem * child(int row);

	/** Gets the number of childs of this item. */
	int childCount() const;

	/** Gets the number of rows of this item. */
	int row() const;

	/** Gets the row index (if any) given its file name. */
	int childRow(const QString & fileName) const;

	/** Gets this item parent item if any. */
	FileSearchItem * parent() const;

	/** Helper function, equivalent to mediaInfo().fileName() */
	QString fileName() const;

	const MediaInfo & mediaInfo() const;
	void setMediaInfo(const MediaInfo & mediaInfo);

	/** Tells if this item is a directory or a file. */
	bool isDir() const;

private:

	/** Data (filename mostly) contained inside this item. */
	MediaInfo _mediaInfo;

	/** Child(s) of this item if any. */
	QList<FileSearchItem *> _childItems;

	/** Parent item of this item. */
	FileSearchItem * _parentItem;

	/** This is being populated or not. */
	bool _populatingChildren;

	mutable int _isDir;
};

#endif	//FILESEARCHITEM_H
