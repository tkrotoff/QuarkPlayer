/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2004-2007  Wengo
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

#include "ThumbnailManager.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QEvent>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

struct ThumbnailEvent : public QEvent {

	ThumbnailEvent(const QString & path, const QImage & image)
		: QEvent(User),
		_path(path),
		_image(image) {
	}

	QString _path;
	QImage _image;
};


static QImage generateThumbnailForPath(const QString & path, int size) {
	QImage image;
	if (!image.load(path)) {
		return QImage();
	}

	if (image.width() > size || image.height() > size) {
		image = image.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	return image;
}

ThumbnailManager::ThumbnailManager(QObject * parent)
	: QThread(parent),
	_size(48),
	_clearing(false) {
}

ThumbnailManager::~ThumbnailManager() {
	clear();
	wait();
}

void ThumbnailManager::setSize(int size) {
	QMutexLocker lock(&_mutex);
	_size = size;
	//_filePixmap = DesktopService::getInstance()->desktopIconPixmap(DesktopService::FileIcon, size);
}

QPixmap ThumbnailManager::thumbnail(const QFileInfo & fileInfo) {
	QString path = fileInfo.absoluteFilePath();
	ThumbnailMap::const_iterator it = _map.find(path);
	if (it != _map.end()) {
		return it.value();
	}

	/*if (fileInfo.isDir()) {
		QPixmap pixmap = DesktopService::getInstance()->pixmapForPath(path, _size);
		_map[path] = pixmap;
		return pixmap;
	}*/

	QMutexLocker lock(&_mutex);
	_clearing = false;
	if (!_pendingQueue.contains(path)) {
		_pendingQueue.enqueue(path);
		qDebug() << __FUNCTION__ << "Queuing:" << path;
		if (!isRunning()) {
			start();
		}
	} else {
		qDebug() << __FUNCTION__ << path << "already in queue";
	}
	return _filePixmap;
}

void ThumbnailManager::run() {
	while (true) {
		QString path;
		{
			QMutexLocker lock(&_mutex);
			if (_pendingQueue.size() == 0) {
				qDebug() << __FUNCTION__ << "Stopped by end of queue";
				return;
			}
			path = _pendingQueue.dequeue();
		}

		QImage image = generateThumbnailForPath(path, _size);
		if (image.isNull()) {
			continue;
		}

		{
			QMutexLocker lock(&_mutex);
			if (_clearing) {
				qDebug() << __FUNCTION__ << "Stopped by _clearing";
				return;
			}
		}
		ThumbnailEvent * event = new ThumbnailEvent(path, image);
		QCoreApplication::postEvent(this, event);
	}
}

void ThumbnailManager::customEvent(QEvent * event) {
	Q_ASSERT(event->type() == QEvent::User);

	ThumbnailEvent * thumbnailEvent = static_cast<ThumbnailEvent *>(event);

	QString path = thumbnailEvent->_path;
	QFileInfo fileInfo(path);
	_map[path] = QPixmap::fromImage(thumbnailEvent->_image);
	thumbnailUpdated(fileInfo);
}

void ThumbnailManager::clear() {
	_map.clear();
	QMutexLocker lock(&_mutex);
	_pendingQueue.clear();
	_clearing = true;
}
