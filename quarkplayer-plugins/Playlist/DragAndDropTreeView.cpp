/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "DragAndDropTreeView.h"

#include "PlaylistWidget.h"
#include "PlaylistModel.h"
#include "PlaylistFilter.h"
#include "PlaylistLogger.h"

#include <quarkplayer/config/Config.h>

#include <quarkplayer-plugins/ConfigWindow/PlaylistConfig.h>

#include <MediaInfoFetcher/MediaInfoFetcher.h>

#include <MediaInfoWindow/MediaInfoWindow.h>

#include <TkUtil/TkAction.h>
#include <TkUtil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

#include <QtCore/QCoreApplication>

DragAndDropTreeView::DragAndDropTreeView(PlaylistWidget * playlistWidget)
	: QTreeView(playlistWidget) {

	_playlistWidget = playlistWidget;
	_playlistModel = _playlistWidget->playlistModel();
	_playlistFilter = _playlistWidget->playlistFilter();

	_mediaInfoWindow = NULL;

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

	connect(_playlistWidget->uuidAction("Playlist.PlayItem"), SIGNAL(triggered()), SLOT(playItem()));
	connect(_playlistWidget->uuidAction("Playlist.RemoveItem"), SIGNAL(triggered()), SLOT(clearSelection()));
	connect(_playlistWidget->uuidAction("Playlist.GetInfo"), SIGNAL(triggered()), SLOT(viewMediaInfo()));
	connect(_playlistWidget->uuidAction("Playlist.OpenDir"), SIGNAL(triggered()), SLOT(openDir()));

	RETRANSLATE(this);
	retranslate();
}

DragAndDropTreeView::~DragAndDropTreeView() {
}

void DragAndDropTreeView::contextMenuEvent(QContextMenuEvent * event) {
	QMenu menu(this);
	menu.addAction(_playlistWidget->uuidAction("Playlist.PlayItem"));
	menu.addAction(_playlistWidget->uuidAction("Playlist.RemoveItem"));
	menu.addSeparator();
	menu.addAction(_playlistWidget->uuidAction("Playlist.GetInfo"));
	menu.addAction(_playlistWidget->uuidAction("Playlist.OpenDir"));
	menu.exec(event->globalPos());
}

void DragAndDropTreeView::mousePressEvent(QMouseEvent * event) {
	//When the user clicks on this playlist, it becomes the only active one
	PlaylistConfig::instance().setActivePlaylist(_playlistWidget->uuid());

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
	Q_ASSERT(app);

	_playlistWidget->addUuidAction("Playlist.PlayItem", new QAction(app));
	_playlistWidget->addUuidAction("Playlist.RemoveItem", new QAction(app));
	TkAction * action = new TkAction(app, tr("Ctrl+I"), tr("Alt+3"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	_playlistWidget->addUuidAction("Playlist.GetInfo", action);
	_playlistWidget->addUuidAction("Playlist.OpenDir", new QAction(app));
}

void DragAndDropTreeView::retranslate() {
	_playlistWidget->uuidAction("Playlist.PlayItem")->setText(tr("Play"));
	_playlistWidget->uuidAction("Playlist.RemoveItem")->setText(tr("Remove from Playlist"));
	_playlistWidget->uuidAction("Playlist.GetInfo")->setText(tr("Get Info..."));
	_playlistWidget->uuidAction("Playlist.OpenDir")->setText(tr("Open Directory..."));

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
		PlaylistDebug() << "Row:" << row;
	}

	//Let's go from the last item to the first one (descending order)
	//this way the algorithm is simplified
	qSort(indexList.begin(), indexList.end(), qGreater<int>());

	int previousRow = -1;
	QList<int> contiguousRows;
	foreach (int currentRow, indexList) {
		PlaylistDebug() << "Current row:" << currentRow;
		if (contiguousRows.isEmpty()) {
			contiguousRows.prepend(currentRow);
		} else {
			if (currentRow == previousRow - 1) {
				contiguousRows.prepend(currentRow);
			} else {
				PlaylistDebug() << "Remove rows:" << contiguousRows.first() << contiguousRows.last();
				_playlistModel->removeRows(contiguousRows.first(), contiguousRows.size());
				contiguousRows.clear();

				PlaylistDebug() << "Remove row:" << currentRow;
				_playlistModel->removeRow(currentRow);
			}
		}

		previousRow = currentRow;
	}

	if (!contiguousRows.isEmpty()) {
		PlaylistDebug() << "Remove rows:" << contiguousRows.first() << contiguousRows.last();
		_playlistModel->removeRows(contiguousRows.first(), contiguousRows.size());
		contiguousRows.clear();
	}
}

void DragAndDropTreeView::viewMediaInfo() {
	PlaylistDebug();

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

void DragAndDropTreeView::openDir() {
	QModelIndexList indexList = selectionModel()->selectedRows();
	if (!indexList.isEmpty()) {
		QModelIndex index(indexList.at(0));
		QString dir = QFileInfo(_playlistModel->mediaInfo(index).fileName()).canonicalPath();
		QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
	}
}
