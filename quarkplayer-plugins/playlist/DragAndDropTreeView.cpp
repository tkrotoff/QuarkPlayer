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

#include "DragAndDropTreeView.h"

#include "PlaylistModel.h"
#include "PlaylistFilter.h"
#include "UuidActionCollection.h"

#include <quarkplayer/config/Config.h>
#include <quarkplayer/config/PlaylistConfig.h>

#include <mediainfowindow/MediaInfoWindow.h>
#include <mediainfowindow/MediaInfoFetcher.h>

#include <tkutil/TkIcon.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

DragAndDropTreeView::DragAndDropTreeView(PlaylistModel * playlistModel, PlaylistFilter * playlistFilter, const QUuid & uuid) {
	_playlistModel = playlistModel;
	_playlistFilter = playlistFilter;

	_mediaInfoWindow = NULL;
	_uuid = uuid;

	setUniformRowHeights(true);
	setDragEnabled(true);
	setAcceptDrops(true);
	setDropIndicatorShown(true);
	setDragDropMode(QAbstractItemView::DragDrop);
	setRootIsDecorated(false);
	setAllColumnsShowFocus(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	//setSortingEnabled(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	populateActionCollection();

	connect(uuidAction("playlistPlayItem"), SIGNAL(triggered()), SLOT(playItem()));
	connect(uuidAction("playlistSendTo"), SIGNAL(triggered()), SLOT(sendTo()));
	connect(uuidAction("playlistDeleteItem"), SIGNAL(triggered()), SLOT(clearSelection()));
	connect(uuidAction("playlistRateItem"), SIGNAL(triggered()), SLOT(rateItem()));
	connect(uuidAction("playlistViewMediaInfo"), SIGNAL(triggered()), SLOT(viewMediaInfo()));

	RETRANSLATE(this);
	retranslate();
}

DragAndDropTreeView::~DragAndDropTreeView() {
}

void DragAndDropTreeView::mousePressEvent(QMouseEvent * event) {
	//When the user clicks on this playlist, it becomes the only active one
	PlaylistConfig::instance().setActivePlaylist(_uuid);

	if (event->button() == Qt::RightButton) {
		showMenu(event->pos());
	}

	else if (event->button() == Qt::LeftButton) {
		//This should be an internal move
		//since we are doing a drag&drop within the playlist, not from outside
		setDragDropMode(QAbstractItemView::InternalMove);
		QTreeView::mousePressEvent(event);
	}
}

void DragAndDropTreeView::dragEvent(QDropEvent * event) {
	if (event->source() == this) {
		//This should be an internal move
		//since we are doing a drag&drop within the playlist, not from outside
		setDragDropMode(QAbstractItemView::InternalMove);
	} else {
		setDragDropMode(QAbstractItemView::DragDrop);
	}
}

void DragAndDropTreeView::dropEvent(QDropEvent * event) {
	dragEvent(event);
	QTreeView::dropEvent(event);
}

void DragAndDropTreeView::dragEnterEvent(QDragEnterEvent * event) {
	dragEvent(event);
	QTreeView::dragEnterEvent(event);
}

void DragAndDropTreeView::dragMoveEvent(QDragMoveEvent * event) {
	dragEvent(event);
	QTreeView::dragMoveEvent(event);
}

void DragAndDropTreeView::showMenu(const QPoint & pos) {
	QModelIndex index(indexAt(pos));
	if (index.isValid()) {
		QModelIndexList indexList = selectionModel()->selectedRows();
		if (indexList.isEmpty()) {
			setCurrentIndex(index);
		} else {
			int selectedIndexRow = index.row();
			int selectionBeginRow = indexList.at(0).row();
			int selectionEndRow = selectionBeginRow + indexList.size();
			qDebug() << " ";
			qDebug() << __FUNCTION__ << "indexRow:" << selectedIndexRow;
			qDebug() << __FUNCTION__ << "beginRow:" << selectionBeginRow;
			qDebug() << __FUNCTION__ << "endRow:" << selectionEndRow;
			qDebug() << " ";
			if (selectedIndexRow >= selectionBeginRow && selectedIndexRow <= selectionEndRow) {
				//Nothing to do, index is already inside the selection
			} else {
				//Index is outside the selection
				setCurrentIndex(index);
			}
		}

		QMenu * menu = new QMenu(this);
		menu->addAction(uuidAction("playlistPlayItem"));
		//FIXME For the future menu->addAction(uuidAction("playlistSendTo"));
		menu->addAction(uuidAction("playlistDeleteItem"));
		//FIXME For the future menu->addAction(uuidAction("playlistRateItem"));
		menu->addAction(uuidAction("playlistViewMediaInfo"));

		menu->exec(QCursor::pos());
	}
}

void DragAndDropTreeView::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	addUuidAction("playlistPlayItem", new QAction(app));
	addUuidAction("playlistSendTo", new QAction(app));
	addUuidAction("playlistDeleteItem", new QAction(app));
	addUuidAction("playlistRateItem", new QAction(app));
	addUuidAction("playlistViewMediaInfo", new QAction(app));
}

void DragAndDropTreeView::retranslate() {
	uuidAction("playlistPlayItem")->setText(tr("Play"));
	uuidAction("playlistPlayItem")->setIcon(TkIcon("media-playback-start"));

	uuidAction("playlistSendTo")->setText(tr("Sent To"));
	uuidAction("playlistSendTo")->setIcon(TkIcon("text-x-script"));

	uuidAction("playlistDeleteItem")->setText(tr("Delete from Playlist"));
	uuidAction("playlistDeleteItem")->setIcon(TkIcon("edit-delete"));

	uuidAction("playlistRateItem")->setText(tr("Rate"));
	uuidAction("playlistRateItem")->setIcon(TkIcon("rating"));

	uuidAction("playlistViewMediaInfo")->setText(tr("View Media Info..."));
	uuidAction("playlistViewMediaInfo")->setIcon(TkIcon("document-properties"));

	if (_mediaInfoWindow) {
		_mediaInfoWindow->setLanguage(Config::instance().language());
	}
}

void DragAndDropTreeView::playItem() {
	QModelIndexList indexList = selectionModel()->selectedRows();
	if (!indexList.isEmpty()) {
		_playlistFilter->play(indexList.at(0));
	}
}

void DragAndDropTreeView::sendTo() {
	//FIXME this will be for external scripts: like burning, sending via email, whatever...
}

void DragAndDropTreeView::clearSelection() {
	//Algorithm a bit complex
	//It contains optimization, that's why the indexList
	//does not go from the first to the last
	//but from the last to the first
	//This simplifies the algorithm

	QList<int> indexList;
	QModelIndexList indexes = selectionModel()->selectedRows();
	foreach (QModelIndex index, indexes) {
		QModelIndex sourceIndex(_playlistFilter->mapToSource(index));
		int row = sourceIndex.row();
		indexList += row;
		qDebug() << __FUNCTION__ << "Row:" << row;
	}

	//Let's go from the last item to the first one (descending order)
	//this way the algorithm is simplified
	qSort(indexList.begin(), indexList.end(), qGreater<int>());

	int previousRow = -1;
	QList<int> contiguousRows;
	foreach (int currentRow, indexList) {
		qDebug() << __FUNCTION__ << "Current row:" << currentRow;
		if (contiguousRows.isEmpty()) {
			contiguousRows.prepend(currentRow);
		} else {
			if (currentRow == previousRow - 1) {
				contiguousRows.prepend(currentRow);
			} else {
				qDebug() << __FUNCTION__ << "Remove rows:" << contiguousRows.first() << contiguousRows.last();
				_playlistModel->removeRows(contiguousRows.first(), contiguousRows.size());
				contiguousRows.clear();

				qDebug() << __FUNCTION__ << "Remove row:" << currentRow;
				_playlistModel->removeRow(currentRow);
			}
		}

		previousRow = currentRow;
	}

	if (!contiguousRows.isEmpty()) {
		qDebug() << __FUNCTION__ << "Remove rows:" << contiguousRows.first() << contiguousRows.last();
		_playlistModel->removeRows(contiguousRows.first(), contiguousRows.size());
		contiguousRows.clear();
	}
}

void DragAndDropTreeView::rateItem() {
}

void DragAndDropTreeView::viewMediaInfo() {
	_mediaInfoWindow = new MediaInfoWindow(this);

	MediaInfoFetcher * mediaInfoFetcher = new MediaInfoFetcher(this);
	QModelIndexList indexList = selectionModel()->selectedRows();
	if (!indexList.isEmpty()) {
		QModelIndex index(indexList.at(0));
		QModelIndex sourceIndex(_playlistFilter->mapToSource(index));
		mediaInfoFetcher->start(_playlistModel->fileName(sourceIndex), MediaInfoFetcher::ReadStyleAccurate);
	}
	_mediaInfoWindow->setMediaInfoFetcher(mediaInfoFetcher);
	_mediaInfoWindow->setLanguage(Config::instance().language());
	_mediaInfoWindow->show();
}
