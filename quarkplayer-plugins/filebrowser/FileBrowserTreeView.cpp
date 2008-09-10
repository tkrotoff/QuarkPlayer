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

#ifdef FASTDIRMODEL
	#include "FastDirModel.h"
#else
	#include "SimpleDirModel.h"
#endif	//FASTDIRMODEL

#include <quarkplayer/QuarkPlayer.h>

#include <tkutil/LanguageChangeEventFilter.h>
#include <tkutil/ActionCollection.h>
#include <tkutil/TkIcon.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

FileBrowserTreeView::FileBrowserTreeView(QuarkPlayer & quarkPlayer)
	: QTreeView(NULL),
	_quarkPlayer(quarkPlayer) {

	populateActionCollection();

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setContextMenuPolicy(Qt::ActionsContextMenu);

	//Add to playlist
	connect(ActionCollection::action("fileBrowserAddToPlaylist"), SIGNAL(triggered()),
		SLOT(addToPlaylist()));
	addAction(ActionCollection::action("fileBrowserAddToPlaylist"));

	//Play
	connect(ActionCollection::action("fileBrowserPlay"), SIGNAL(triggered()),
		SLOT(play()));
	addAction(ActionCollection::action("fileBrowserPlay"));

	//Refresh action
	/*connect(ActionCollection::action("fileBrowserRefresh"), SIGNAL(triggered()),
		SLOT(refresh()));
	addAction(ActionCollection::action("fileBrowserRefresh"));*/

	RETRANSLATE(this);
	retranslate();
}

FileBrowserTreeView::~FileBrowserTreeView() {
}

void FileBrowserTreeView::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	//ActionCollection::addAction("fileBrowserRefresh", new QAction(app));
	ActionCollection::addAction("fileBrowserAddToPlaylist", new QAction(app));
	ActionCollection::addAction("fileBrowserPlay", new QAction(app));
}

void FileBrowserTreeView::mouseDoubleClickEvent(QMouseEvent * event) {
	QTreeView::mouseDoubleClickEvent(event);
	addToPlaylist();
}

void FileBrowserTreeView::addToPlaylist() {
	QStringList filenames;
	QModelIndexList indexList = selectedIndexes();
	foreach (QModelIndex index, indexList) {
		QFileInfo fileInfo = dirModel()->fileInfo(index);
		//Sometimes, QFileInfo gives us this pattern: C://... that MPlayer does not accept
		filenames += fileInfo.absoluteFilePath().replace("//", "/");
	}
	if (!filenames.isEmpty()) {
		_quarkPlayer.addFilesToPlaylist(filenames);
	}
}

void FileBrowserTreeView::play() {
	QModelIndexList indexList = selectedIndexes();
	if (!indexList.isEmpty()) {
		QModelIndex index = indexList.at(0);
		if (index.isValid()) {
			QFileInfo fileInfo = dirModel()->fileInfo(index);
			if (fileInfo.isFile()) {
				//Sometimes, QFileInfo gives us this pattern: C://... that MPlayer does not accept
				QString slashSlashBugFix = fileInfo.absoluteFilePath().replace("//", "/");
				_quarkPlayer.play(slashSlashBugFix);
			}
		}
	}
}

DirModel * FileBrowserTreeView::dirModel() {
	QAbstractItemModel * itemModel = model();
	return static_cast<DirModel *>(itemModel);
}

void FileBrowserTreeView::retranslate() {
	ActionCollection::action("fileBrowserAddToPlaylist")->setText(tr("Add to Playlist"));
	ActionCollection::action("fileBrowserAddToPlaylist")->setIcon(TkIcon("list-add"));

	ActionCollection::action("fileBrowserPlay")->setText(tr("Play"));
	ActionCollection::action("fileBrowserPlay")->setIcon(TkIcon("media-playback-start"));

	//ActionCollection::action("fileBrowserRefresh")->setText(tr("Refresh"));
	//ActionCollection::action("fileBrowserRefresh")->setIcon(TkIcon("view-refresh"));
}
