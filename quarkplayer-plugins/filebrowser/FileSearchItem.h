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

#include <mediainfowindow/MediaInfo.h>

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

	void setPopulatedChildren(bool populatedChildren);
	bool populatedChildren() const;

	void appendChild(FileSearchItem * newItem);

	FileSearchItem * child(int row);
	int childCount() const;
	int row() const;
	FileSearchItem * parent();

	/** Helper function, equivalent to mediaInfo().fileName() */
	QString fileName() const;

	const MediaInfo & mediaInfo() const;
	void setMediaInfo(const MediaInfo & mediaInfo);

	bool isDir();

private:

	/** Data (filename mostly) contained inside this item. */
	MediaInfo _mediaInfo;

	/** Child(s) of this item if any. */
	QList<FileSearchItem *> _childItems;

	/** Parent item of this item. */
	FileSearchItem * _parentItem;

	bool _populatedChildren;

	int _isDir;

	int _firstFileItemAdded;
};

#endif	//FILESEARCHITEM_H
