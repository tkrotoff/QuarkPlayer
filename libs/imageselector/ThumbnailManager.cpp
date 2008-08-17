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

	ThumbnailEvent(const QString & _path, const QImage & _image)
		: QEvent(User),
		path(_path),
		image(_image) {
	}

	QString path;
	QImage image;
};


static QImage generateThumbnailForPath(const QString & path, int thumbnailSize) {
	QImage image;
	if (!image.load(path)) {
		return QImage();
	}

	if (image.width() > thumbnailSize || image.height() > thumbnailSize) {
		image = image.scaled(thumbnailSize, thumbnailSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	return image;
}


static const int DEFAULT_THUMBNAIL_SIZE = 48;

ThumbnailManager::ThumbnailManager(QObject * parent)
	: QThread(parent) {

	_thumbnailSize = DEFAULT_THUMBNAIL_SIZE;
	_clearing = false;
}

ThumbnailManager::~ThumbnailManager() {
	clear();
	wait();
}

void ThumbnailManager::setThumbnailSize(int size) {
	QMutexLocker lock(&_mutex);
	_thumbnailSize = size;
}

QPixmap ThumbnailManager::thumbnail(const QFileInfo & fileInfo) {
	QString path = fileInfo.absoluteFilePath();
	ThumbnailMap::const_iterator it = _map.find(path);
	if (it != _map.end()) {
		//Returns the cached thumbnail
		return it.value();
	}

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

	//Returns an empty pixmap
	return QPixmap();
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

		QImage image = generateThumbnailForPath(path, _thumbnailSize);
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

	QString path = thumbnailEvent->path;
	_map[path] = QPixmap::fromImage(thumbnailEvent->image);
	QFileInfo fileInfo(path);
	thumbnailUpdated(fileInfo);
}

void ThumbnailManager::clear() {
	_map.clear();
	QMutexLocker lock(&_mutex);
	_pendingQueue.clear();
	_clearing = true;
}
