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

#include <quarkplayer/config/Config.h>

#include <mediainfowindow/MediaInfoWindow.h>
#include <mediainfowindow/MediaInfoFetcher.h>

#include <tkutil/ActionCollection.h>
#include <tkutil/TkIcon.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

DragAndDropTreeView::DragAndDropTreeView(PlaylistModel * playlistModel, PlaylistFilter * playlistFilter) {
	_playlistModel = playlistModel;
	_playlistFilter = playlistFilter;

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

	connect(ActionCollection::action("playlistPlayItem"), SIGNAL(triggered()), SLOT(playItem()));
	connect(ActionCollection::action("playlistSendTo"), SIGNAL(triggered()), SLOT(sendTo()));
	connect(ActionCollection::action("playlistDeleteItem"), SIGNAL(triggered()), SLOT(clearSelection()));
	connect(ActionCollection::action("playlistRateItem"), SIGNAL(triggered()), SLOT(rateItem()));
	connect(ActionCollection::action("playlistViewMediaInfo"), SIGNAL(triggered()), SLOT(viewMediaInfo()));

	RETRANSLATE(this);
	retranslate();
}

DragAndDropTreeView::~DragAndDropTreeView() {
}

void DragAndDropTreeView::mousePressEvent(QMouseEvent * event) {
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
		QModelIndexList indexList = selectedIndexes();
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
		menu->addAction(ActionCollection::action("playlistPlayItem"));
		//FIXME For the future menu->addAction(ActionCollection::action("playlistSendTo"));
		menu->addAction(ActionCollection::action("playlistDeleteItem"));
		//FIXME For the future menu->addAction(ActionCollection::action("playlistRateItem"));
		menu->addAction(ActionCollection::action("playlistViewMediaInfo"));

		menu->exec(QCursor::pos());
	}
}

void DragAndDropTreeView::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("playlistPlayItem", new QAction(app));
	ActionCollection::addAction("playlistSendTo", new QAction(app));
	ActionCollection::addAction("playlistDeleteItem", new QAction(app));
	ActionCollection::addAction("playlistRateItem", new QAction(app));
	ActionCollection::addAction("playlistViewMediaInfo", new QAction(app));
}

void DragAndDropTreeView::retranslate() {
	ActionCollection::action("playlistPlayItem")->setText(tr("Play"));
	ActionCollection::action("playlistPlayItem")->setIcon(TkIcon("media-playback-start"));

	ActionCollection::action("playlistSendTo")->setText(tr("Sent To"));
	ActionCollection::action("playlistSendTo")->setIcon(TkIcon("text-x-script"));

	ActionCollection::action("playlistDeleteItem")->setText(tr("Delete from Playlist"));
	ActionCollection::action("playlistDeleteItem")->setIcon(TkIcon("edit-delete"));

	ActionCollection::action("playlistRateItem")->setText(tr("Rate"));
	ActionCollection::action("playlistRateItem")->setIcon(TkIcon("rating"));

	ActionCollection::action("playlistViewMediaInfo")->setText(tr("View Media Info..."));
	ActionCollection::action("playlistViewMediaInfo")->setIcon(TkIcon("document-properties"));

	if (_mediaInfoWindow) {
		_mediaInfoWindow->setLocale(Config::instance().language());
	}
}

void DragAndDropTreeView::playItem() {
	QModelIndexList indexList = selectedIndexes();
	if (!indexList.isEmpty()) {
		_playlistFilter->play(indexList.at(0));
	}
}

void DragAndDropTreeView::sendTo() {
	//FIXME this will be for external scripts: like burning, sending via email, whatever...
}

void DragAndDropTreeView::clearSelection() {
	QModelIndexList indexList = selectedIndexes();
	QList<int> rows;
	foreach (QModelIndex index, indexList) {
		int row = index.row();
		if (!rows.contains(row)) {
			rows += row;
		}
	}

	if (!rows.isEmpty()) {
		_playlistModel->removeRows(rows.first(), rows.size());
	}
}

void DragAndDropTreeView::rateItem() {
}

void DragAndDropTreeView::viewMediaInfo() {
	_mediaInfoWindow = new MediaInfoWindow(this);

	MediaInfoFetcher * mediaInfoFetcher = new MediaInfoFetcher(this);
	QModelIndexList indexList = selectedIndexes();
	if (!indexList.isEmpty()) {
		QModelIndex index(indexList.at(0));
		QModelIndex sourceIndex(_playlistFilter->mapToSource(index));
		mediaInfoFetcher->start(_playlistModel->filename(sourceIndex));
	}
	_mediaInfoWindow->setMediaInfoFetcher(mediaInfoFetcher);
	_mediaInfoWindow->setLocale(Config::instance().language());
	_mediaInfoWindow->show();
}
