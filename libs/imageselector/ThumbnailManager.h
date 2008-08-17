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

#ifndef THUMBNAILMANAGER_H
#define THUMBNAILMANAGER_H

#include <QtGui/QPixmap>

#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QQueue>
#include <QtCore/QThread>

class QEvent;
class QFileInfo;

/**
 * This class generates thumbnails for images.
 *
 * Thumbnails are generated in a separated thread and cached.
 *
 * @author Aurélien Gâteau
 * @author Tanguy Krotoff
 */
class ThumbnailManager : public QThread {
	Q_OBJECT
public:

	ThumbnailManager(QObject * parent);

	~ThumbnailManager();

	/**
	 * Generates a thumbnail given a file.
	 *
	 * @return the thumbnail if already available; a null pixmap otherwise
	 */
	QPixmap thumbnail(const QFileInfo & fileInfo);

	/**
	 * Clears the thumbnail cache.
	 */
	void clear();

	/**
	 * Sets the thumbnail size in pixel.
	 */
	void setThumbnailSize(int size);

signals:

	/**
	 * Emitted whenever a thumbnail requested has been generated.
	 *
	 * @see thumbnail()
	 */
	void thumbnailUpdated(const QFileInfo & fileInfo);

private:

	void run();

	void customEvent(QEvent * event);

	typedef QMap<QString, QPixmap> ThumbnailMap;
	ThumbnailMap _map;

	/** Thumbnail size. */
	int _thumbnailSize;

	/** This mutex protects _pendingQueue and _clearing. */
	QMutex _mutex;

	typedef QQueue<QString> PendingThumbnailQueue;
	PendingThumbnailQueue _pendingQueue;

	/** Set to true if clear() has been called. */
	bool _clearing;
};

#endif	//THUMBNAILMANAGER_H
