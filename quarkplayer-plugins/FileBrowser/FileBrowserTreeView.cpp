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

#include "FileBrowserTreeView.h"

#include "FileBrowserWidget.h"
#include "FileSearchModel.h"
#include "FileBrowserLogger.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/PluginManager.h>

#include <quarkplayer-plugins/Playlist/PlaylistWidget.h>

#include <MediaInfoWindow/MediaInfoWindow.h>

#include <MediaInfoFetcher/MediaInfoFetcher.h>

#include <TkUtil/LanguageChangeEventFilter.h>

#include <phonon/mediaobject.h>

#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>

#include <QtCore/QFileInfo>

//For INT_MAX
#include <climits>

FileBrowserTreeView::FileBrowserTreeView(FileBrowserWidget * fileBrowserWidget)
	: QTreeView(NULL) {

	_fileBrowserWidget = fileBrowserWidget;

	populateActionCollection();

	setUniformRowHeights(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(this, SIGNAL(activated(const QModelIndex &)),
		SLOT(activated(const QModelIndex &)));

	connect(_actions["FileBrowser.AddToPlaylist"], SIGNAL(triggered()),
		SLOT(addToPlaylist()));
	connect(_actions["FileBrowser.Play"], SIGNAL(triggered()),
		SLOT(play()));
	connect(_actions["FileBrowser.GetInfo"], SIGNAL(triggered()),
		SLOT(viewMediaInfo()));
	connect(_actions["FileBrowser.OpenDir"], SIGNAL(triggered()),
		SLOT(openDir()));

	RETRANSLATE(this);
	retranslate();
}

FileBrowserTreeView::~FileBrowserTreeView() {
}

void FileBrowserTreeView::contextMenuEvent(QContextMenuEvent * event) {
	QMenu menu(this);
	menu.addAction(_actions["FileBrowser.AddToPlaylist"]);
	menu.addAction(_actions["FileBrowser.Play"]);
	menu.addSeparator();
	menu.addAction(_actions["FileBrowser.GetInfo"]);
	menu.addAction(_actions["FileBrowser.OpenDir"]);
	menu.exec(event->globalPos());
}

void FileBrowserTreeView::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();
	Q_ASSERT(app);

	_actions.add("FileBrowser.AddToPlaylist", new QAction(app));
	_actions.add("FileBrowser.Play", new QAction(app));
	_actions.add("FileBrowser.GetInfo", new QAction(app));
	_actions.add("FileBrowser.OpenDir", new QAction(app));
}

void FileBrowserTreeView::activated(const QModelIndex & index) {
	QFileInfo fileInfo = this->fileInfo(index);
	FileBrowserDebug() << fileInfo.fileName() << index.row() << index.column();

	//Do not add directories to the playlist
	if (!fileInfo.isDir()) {
		addToPlaylist();
	}
}

void FileBrowserTreeView::addToPlaylist() {
	QStringList files;
	QModelIndexList indexList = selectionModel()->selectedRows();
	foreach (QModelIndex index, indexList) {
		QFileInfo fileInfo(this->fileInfo(index));
		//Sometimes, QFileInfo gives us this pattern: C://... that MPlayer does not accept
		files += fileInfo.absoluteFilePath().replace("//", "/");
	}
	if (!files.isEmpty()) {
		PlaylistWidgetFactory::playlistWidget()->addFilesToPlaylist(files);
	}
}

void FileBrowserTreeView::play() {
	QModelIndexList indexList(selectionModel()->selectedRows());
	if (!indexList.isEmpty()) {
		QModelIndex index(indexList.at(0));
		if (index.isValid()) {
			QFileInfo fileInfo(this->fileInfo(index));
			if (fileInfo.isFile()) {
				//Sometimes, QFileInfo gives us this pattern: C://... that MPlayer does not accept
				QString slashSlashBugFix(fileInfo.absoluteFilePath().replace("//", "/"));
				_fileBrowserWidget->quarkPlayer().play(slashSlashBugFix);
			}
		}
	}
}

void FileBrowserTreeView::retranslate() {
	_actions["FileBrowser.AddToPlaylist"]->setText(tr("Add to Playlist"));
	_actions["FileBrowser.Play"]->setText(tr("Play"));
	_actions["FileBrowser.GetInfo"]->setText(tr("Get Info..."));
	_actions["FileBrowser.OpenDir"]->setText(tr("Open Directory..."));
}

QFileInfo FileBrowserTreeView::fileInfo(const QModelIndex & index) const {
	QFileInfo tmp;

	const QAbstractItemModel * model = index.model();
	if (const FileSearchModel * fileSearchModel = dynamic_cast<const FileSearchModel *>(model)) {
		tmp = fileSearchModel->fileInfo(index);
	} else {
		FileBrowserCritical() << "Couldn't cast to FileSearchModel";
	}

	return tmp;
}

void FileBrowserTreeView::viewMediaInfo() {
	static MediaInfoWindow * mediaInfoWindow = new MediaInfoWindow(this);

	MediaInfoFetcher * mediaInfoFetcher = new MediaInfoFetcher(this);
	QModelIndexList indexList = selectionModel()->selectedRows();
	if (!indexList.isEmpty()) {
		QModelIndex index(indexList.at(0));
		mediaInfoFetcher->start(MediaInfo(fileInfo(index).absoluteFilePath()),
			MediaInfoFetcher::ReadStyleAccurate);
	}
	mediaInfoWindow->setMediaInfoFetcher(mediaInfoFetcher);
	mediaInfoWindow->setLanguage(Config::instance().language());
	mediaInfoWindow->show();
}

void FileBrowserTreeView::openDir() {
	QModelIndexList indexList = selectionModel()->selectedRows();
	if (!indexList.isEmpty()) {
		QModelIndex index(indexList.at(0));
		QString dir = fileInfo(index).canonicalPath();
		QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
	}
}
