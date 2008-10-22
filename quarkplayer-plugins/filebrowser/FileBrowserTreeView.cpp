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

#include "FileBrowserTreeView.h"

#include "FileSearchModel.h"
#include "UuidActionCollection.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/config/Config.h>

#include <mediainfowindow/MediaInfoWindow.h>
#include <mediainfowindow/MediaInfoFetcher.h>

#include <tkutil/LanguageChangeEventFilter.h>
#include <tkutil/TkIcon.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

FileBrowserTreeView::FileBrowserTreeView(QuarkPlayer & quarkPlayer)
	: QTreeView(NULL),
	_quarkPlayer(quarkPlayer) {

	populateActionCollection();

	setUniformRowHeights(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setContextMenuPolicy(Qt::ActionsContextMenu);

	connect(this, SIGNAL(clicked(const QModelIndex &)),
		SLOT(clicked(const QModelIndex &)));

	//Add to playlist
	connect(uuidAction("fileBrowserAddToPlaylist"), SIGNAL(triggered()),
		SLOT(addToPlaylist()));
	addAction(uuidAction("fileBrowserAddToPlaylist"));

	//Play
	connect(uuidAction("fileBrowserPlay"), SIGNAL(triggered()),
		SLOT(play()));
	addAction(uuidAction("fileBrowserPlay"));

	//View Media Info...
	connect(uuidAction("fileBrowserViewMediaInfo"), SIGNAL(triggered()),
		SLOT(viewMediaInfo()));
	addAction(uuidAction("fileBrowserViewMediaInfo"));

	RETRANSLATE(this);
	retranslate();
}

FileBrowserTreeView::~FileBrowserTreeView() {
}

void FileBrowserTreeView::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	addUuidAction("fileBrowserAddToPlaylist", new QAction(app));
	addUuidAction("fileBrowserPlay", new QAction(app));
	addUuidAction("fileBrowserViewMediaInfo", new QAction(app));
}

void FileBrowserTreeView::mouseDoubleClickEvent(QMouseEvent * event) {
	QTreeView::mouseDoubleClickEvent(event);
	addToPlaylist();
}

void FileBrowserTreeView::clicked(const QModelIndex & index) {
	QFileInfo fileInfo = this->fileInfo(index);
	qDebug() << __FUNCTION__ << fileInfo.fileName() << index.row() << index.column();

	//Cannot play a directory
	uuidAction("fileBrowserPlay")->setEnabled(!fileInfo.isDir());
}

void FileBrowserTreeView::addToPlaylist() {
	QStringList filenames;
	QModelIndexList indexList = selectionModel()->selectedRows();
	foreach (QModelIndex index, indexList) {
		QFileInfo fileInfo = this->fileInfo(index);
		//Sometimes, QFileInfo gives us this pattern: C://... that MPlayer does not accept
		filenames += fileInfo.absoluteFilePath().replace("//", "/");
	}
	if (!filenames.isEmpty()) {
		_quarkPlayer.addFilesToPlaylist(filenames);
	}
}

void FileBrowserTreeView::play() {
	QModelIndexList indexList = selectionModel()->selectedRows();
	if (!indexList.isEmpty()) {
		QModelIndex index = indexList.at(0);
		if (index.isValid()) {
			QFileInfo fileInfo = this->fileInfo(index);
			if (fileInfo.isFile()) {
				//Sometimes, QFileInfo gives us this pattern: C://... that MPlayer does not accept
				QString slashSlashBugFix = fileInfo.absoluteFilePath().replace("//", "/");
				_quarkPlayer.play(slashSlashBugFix);
			}
		}
	}
}

void FileBrowserTreeView::retranslate() {
	uuidAction("fileBrowserAddToPlaylist")->setText(tr("Add to Playlist"));
	uuidAction("fileBrowserAddToPlaylist")->setIcon(TkIcon("list-add"));

	uuidAction("fileBrowserPlay")->setText(tr("Play"));
	uuidAction("fileBrowserPlay")->setIcon(TkIcon("media-playback-start"));

	uuidAction("fileBrowserViewMediaInfo")->setText(tr("View Media Info..."));
	uuidAction("fileBrowserViewMediaInfo")->setIcon(TkIcon("document-properties"));
}

QFileInfo FileBrowserTreeView::fileInfo(const QModelIndex & index) const {
	QFileInfo tmp;

	const QAbstractItemModel * model = index.model();
	if (const QFileSystemModel * dirModel = dynamic_cast<const QFileSystemModel *>(model)) {
		tmp = dirModel->fileInfo(index);
	} else if (const FileSearchModel * fileSearchModel = dynamic_cast<const FileSearchModel *>(model)) {
		tmp = fileSearchModel->fileInfo(index);
	}

	return tmp;
}

void FileBrowserTreeView::viewMediaInfo() {
	static MediaInfoWindow * mediaInfoWindow = new MediaInfoWindow(this);

	MediaInfoFetcher * mediaInfoFetcher = new MediaInfoFetcher(this);
	QModelIndexList indexList = selectionModel()->selectedRows();
	if (!indexList.isEmpty()) {
		QModelIndex index(indexList.at(0));
		mediaInfoFetcher->start(fileInfo(index).absoluteFilePath());
	}
	mediaInfoWindow->setMediaInfoFetcher(mediaInfoFetcher);
	mediaInfoWindow->setLocale(Config::instance().language());
	mediaInfoWindow->show();
}
