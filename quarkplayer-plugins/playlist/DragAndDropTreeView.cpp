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
#include <tkutil/TkAction.h>
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
	setContextMenuPolicy(Qt::ActionsContextMenu);

	populateActionCollection();

	connect(uuidAction("Playlist.PlayItem"), SIGNAL(triggered()), SLOT(playItem()));
	addAction(uuidAction("Playlist.PlayItem"));
	connect(uuidAction("Playlist.SendTo"), SIGNAL(triggered()), SLOT(sendTo()));
	//addAction(uuidAction("Playlist.SendTo"));
	connect(uuidAction("Playlist.DeleteItem"), SIGNAL(triggered()), SLOT(clearSelection()));
	addAction(uuidAction("Playlist.DeleteItem"));
	connect(uuidAction("Playlist.RateItem"), SIGNAL(triggered()), SLOT(rateItem()));
	//addAction(uuidAction("Playlist.RateItem"));
	connect(uuidAction("Playlist.ViewMediaInfo"), SIGNAL(triggered()), SLOT(viewMediaInfo()));
	addAction(uuidAction("Playlist.ViewMediaInfo"));

	RETRANSLATE(this);
	retranslate();
}

DragAndDropTreeView::~DragAndDropTreeView() {
}

void DragAndDropTreeView::mousePressEvent(QMouseEvent * event) {
	//When the user clicks on this playlist, it becomes the only active one
	PlaylistConfig::instance().setActivePlaylist(_uuid);

	if (event->button() == Qt::RightButton) {
		QModelIndex index(indexAt(event->pos()));
		if (index.isValid()) {
			setCurrentIndex(index);
		}
	}

	if (event->button() == Qt::LeftButton) {
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

void DragAndDropTreeView::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	addUuidAction("Playlist.PlayItem", new QAction(app));
	addUuidAction("Playlist.SendTo", new QAction(app));
	addUuidAction("Playlist.DeleteItem", new QAction(app));
	addUuidAction("Playlist.RateItem", new QAction(app));
	TkAction * action = new TkAction(app, tr("Ctrl+I"), tr("Alt+3"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	addUuidAction("Playlist.ViewMediaInfo", action);
}

void DragAndDropTreeView::retranslate() {
	uuidAction("Playlist.PlayItem")->setText(tr("Play"));
	uuidAction("Playlist.PlayItem")->setIcon(TkIcon("media-playback-start"));

	uuidAction("Playlist.SendTo")->setText(tr("Sent To"));
	uuidAction("Playlist.SendTo")->setIcon(TkIcon("text-x-script"));

	uuidAction("Playlist.DeleteItem")->setText(tr("Delete from Playlist"));
	uuidAction("Playlist.DeleteItem")->setIcon(TkIcon("edit-delete"));

	uuidAction("Playlist.RateItem")->setText(tr("Rate"));
	uuidAction("Playlist.RateItem")->setIcon(TkIcon("rating"));

	uuidAction("Playlist.ViewMediaInfo")->setText(tr("View Media Info..."));
	uuidAction("Playlist.ViewMediaInfo")->setIcon(TkIcon("document-properties"));

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
	qDebug() << __FUNCTION__;

	_mediaInfoWindow = new MediaInfoWindow(this);

	MediaInfoFetcher * mediaInfoFetcher = new MediaInfoFetcher(this);
	QModelIndexList indexList = selectionModel()->selectedRows();
	if (!indexList.isEmpty()) {
		QModelIndex index(indexList.at(0));
		QModelIndex sourceIndex(_playlistFilter->mapToSource(index));
		mediaInfoFetcher->start(_playlistModel->mediaInfo(sourceIndex), MediaInfoFetcher::ReadStyleAccurate);
	}
	_mediaInfoWindow->setMediaInfoFetcher(mediaInfoFetcher);
	_mediaInfoWindow->setLanguage(Config::instance().language());
	_mediaInfoWindow->show();
}
