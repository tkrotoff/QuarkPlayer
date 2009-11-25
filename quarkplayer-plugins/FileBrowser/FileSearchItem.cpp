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

#include <tkutil/TkFile.h>

#include <QtCore/QStringList>
#include <QtCore/QDebug>

FileSearchItem::FileSearchItem(const QString & filename, FileSearchItem * parent) {
	_mediaInfo = MediaInfo(filename);
	_parentItem = parent;
	_populatedChildren = false;
	_isDir = -1;
	_firstFileItemAdded = -1;
}

FileSearchItem::~FileSearchItem() {
	qDeleteAll(_childItems);
}

void FileSearchItem::setPopulatedChildren(bool populatedChildren) {
	_populatedChildren = populatedChildren;
}

bool FileSearchItem::populatedChildren() const {
	return _populatedChildren;
}

void FileSearchItem::appendChild(FileSearchItem * newItem) {
	_childItems.append(newItem);
}

FileSearchItem * FileSearchItem::child(int row) {
	return _childItems.value(row);
}

int FileSearchItem::childCount() const {
	return _childItems.count();
}

FileSearchItem * FileSearchItem::parent() const {
	return _parentItem;
}

int FileSearchItem::row() const {
	if (_parentItem) {
		return _parentItem->_childItems.indexOf(const_cast<FileSearchItem *>(this));
	}

	return 0;
}

int FileSearchItem::childRow(const QString & filename) const {
	int row = 0;
	foreach (FileSearchItem * item, _childItems) {
		Q_ASSERT(item);
		if (item->fileName() == filename) {
			row++;
			break;
		}
	}
	return row;
}

QString FileSearchItem::fileName() const {
	return _mediaInfo.fileName();
}

const MediaInfo & FileSearchItem::mediaInfo() const {
	return _mediaInfo;
}

void FileSearchItem::setMediaInfo(const MediaInfo & mediaInfo) {
	_mediaInfo = mediaInfo;
}

bool FileSearchItem::isDir() const {
	if (_isDir == -1) {
		//Avoid some computations
		//since _isDir is an attribute of this class
		_isDir = TkFile::isDir(_mediaInfo.fileName());
	}
	return _isDir;
}

class FileSearchModelSorter {
public:

	bool operator()(const FileSearchItem * leftItem, const FileSearchItem * rightItem) const {
		//Place directories before files
		//FIXME Not under MacOS X ?
		bool left = leftItem->isDir();
		bool right = rightItem->isDir();
		if (left ^ right) {
			return left;
		}
		return false;
	}

private:

};

void FileSearchItem::sortChildren() {
	FileSearchModelSorter modelSorter;
	qStableSort(_childItems.begin(), _childItems.end(), modelSorter);
}
