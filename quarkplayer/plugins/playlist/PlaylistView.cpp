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

#include "PlaylistView.h"

#include "PlaylistModel.h"

#include "ui_PlaylistView.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/FileExtensions.h>
#include <quarkplayer/config/Config.h>

#include <tkutil/ActionCollection.h>
#include <tkutil/TkIcon.h>
#include <tkutil/FindFiles.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtGui/QtGui>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(PlaylistView, PlaylistViewFactory);

PluginInterface * PlaylistViewFactory::create(QuarkPlayer & quarkPlayer) const {
	return new PlaylistView(quarkPlayer);
}

PlaylistView::PlaylistView(QuarkPlayer & quarkPlayer)
	: PluginInterface(quarkPlayer),
	QWidget(NULL) {

	_ui = new Ui::PlaylistView();
	_ui->setupUi(this);

	_ui->treeView->setUniformRowHeights(true);
	_ui->treeView->setDragEnabled(true);
	_ui->treeView->setAcceptDrops(true);
	_ui->treeView->setDropIndicatorShown(true);
	_ui->treeView->setDragDropMode(QAbstractItemView::DragDrop);
	_ui->treeView->setRootIsDecorated(false);
	_ui->treeView->setAllColumnsShowFocus(true);
	_ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	_ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	//_ui->treeView->setSortingEnabled(true);

	_playlistModel = new PlaylistModel(this, quarkPlayer);
	connect(_playlistModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
		SLOT(resizeColumnsToContents()));
	connect(_ui->treeView, SIGNAL(activated(const QModelIndex &)),
		_playlistModel, SLOT(play(const QModelIndex &)));

	_ui->treeView->setModel(_playlistModel);
	for (int i = 0; i < _playlistModel->columnCount(); i++) {
		_ui->treeView->resizeColumnToContents(i);
	}

	layout()->setMargin(0);
	layout()->setSpacing(0);

	populateActionCollection();
	createPlaylistToolBar();

	RETRANSLATE(this);
	retranslate();

	//Add to the main window
	QDockWidget * dockWidget = new QDockWidget(tr("Playlist"));
	quarkPlayer.mainWindow().addPlaylistDockWidget(dockWidget);
	dockWidget->setWidget(this);

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));
}

PlaylistView::~PlaylistView() {
}

void PlaylistView::createPlaylistToolBar() {
	_playlistToolBar = new QToolBar(NULL);
	//_playlistToolBar->setIconSize(QSize(16, 16));
	layout()->addWidget(_playlistToolBar);

	_playlistToolBar->addAction(ActionCollection::action("playlistOpen"));
	_playlistToolBar->addAction(ActionCollection::action("playlistSave"));
	_playlistToolBar->addSeparator();

	//We have to use a QToolButton instead of a QAction,
	//otherwise we cannot use QToolButton::InstantPopup :/
	QToolButton * addButton = new QToolButton();
	addButton->setPopupMode(QToolButton::InstantPopup);
	addButton->setDefaultAction(ActionCollection::action("playlistAdd"));
	_playlistToolBar->addWidget(addButton);

	QToolButton * removeButton = new QToolButton();
	removeButton->setPopupMode(QToolButton::InstantPopup);
	removeButton->setDefaultAction(ActionCollection::action("playlistRemove"));
	_playlistToolBar->addWidget(removeButton);

	QMenu * addMenu = new QMenu();
	addMenu->addAction(ActionCollection::action("playlistAddFiles"));
	connect(ActionCollection::action("playlistAddFiles"), SIGNAL(triggered()), SLOT(addFiles()));
	addMenu->addAction(ActionCollection::action("playlistAddDirectory"));
	connect(ActionCollection::action("playlistAddDirectory"), SIGNAL(triggered()), SLOT(addDir()));
	addMenu->addAction(ActionCollection::action("playlistAddURL"));
	connect(ActionCollection::action("playlistAddURL"), SIGNAL(triggered()), SLOT(addURL()));
	addButton->setMenu(addMenu);

	QMenu * removeMenu = new QMenu();
	removeMenu->addAction(ActionCollection::action("playlistRemoveSelected"));
	connect(ActionCollection::action("playlistRemoveSelected"), SIGNAL(triggered()), _playlistModel, SLOT(clearSelection()));
	removeMenu->addAction(ActionCollection::action("playlistRemoveAll"));
	connect(ActionCollection::action("playlistRemoveAll"), SIGNAL(triggered()), _playlistModel, SLOT(clear()));
	removeButton->setMenu(removeMenu);
}

void PlaylistView::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("playlistOpen", new QAction(app));
	ActionCollection::addAction("playlistSave", new QAction(app));

	ActionCollection::addAction("playlistAdd", new QAction(app));
	ActionCollection::addAction("playlistAddFiles", new QAction(app));
	ActionCollection::addAction("playlistAddDirectory", new QAction(app));
	ActionCollection::addAction("playlistAddURL", new QAction(app));

	ActionCollection::addAction("playlistRemove", new QAction(app));
	ActionCollection::addAction("playlistRemoveSelected", new QAction(app));
	ActionCollection::addAction("playlistRemoveAll", new QAction(app));

	ActionCollection::addAction("playlistRemoveSelected", new QAction(app));
	ActionCollection::addAction("playlistRemoveAll", new QAction(app));

}

void PlaylistView::retranslate() {
	ActionCollection::action("playlistOpen")->setText(tr("Open Playlist"));
	ActionCollection::action("playlistOpen")->setIcon(TkIcon("document-open"));

	ActionCollection::action("playlistSave")->setText(tr("Save Playlist"));
	ActionCollection::action("playlistSave")->setIcon(TkIcon("document-save"));

	ActionCollection::action("playlistAdd")->setText(tr("Add..."));
	ActionCollection::action("playlistAdd")->setIcon(TkIcon("list-add"));

	ActionCollection::action("playlistAddFiles")->setText(tr("Add Files"));
	ActionCollection::action("playlistAddDirectory")->setText(tr("Add Directory"));
	ActionCollection::action("playlistAddURL")->setText(tr("Add URL"));

	ActionCollection::action("playlistRemove")->setText(tr("Remove..."));
	ActionCollection::action("playlistRemove")->setIcon(TkIcon("list-remove"));

	ActionCollection::action("playlistRemoveSelected")->setText(tr("Remove Selected"));
	ActionCollection::action("playlistRemoveAll")->setText(tr("Remove All"));
}

void PlaylistView::addFiles() {
	QStringList files = TkFileDialog::getOpenFileNames(this, tr("Select Audio/Video Files"), Config::instance().lastDirectoryUsed());
	_playlistModel->addFiles(files);
}

void PlaylistView::addDir() {
	QStringList files;

	QString dir = TkFileDialog::getExistingDirectory(this, tr("Select DVD folder"),
			Config::instance().lastDirectoryUsed());

	QStringList tmp(FindFiles::findAllFiles(dir));
	foreach (QString file, tmp) {
		QFileInfo fileInfo(file);
		bool isMultimediaFile = FileExtensions::multimedia().contains(fileInfo.suffix(), Qt::CaseInsensitive);
		if (isMultimediaFile) {
			files << file;
		}
	}

	_playlistModel->addFiles(files);
}

void PlaylistView::addURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));

	QStringList files;
	files << url;

	_playlistModel->addFiles(files);
}

void PlaylistView::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	//aboutToFinish -> let's play the next track
	connect(mediaObject, SIGNAL(aboutToFinish()), SLOT(aboutToFinish()));

	//Next track
	disconnect(ActionCollection::action("nextTrack"), 0, 0, 0);
	connect(ActionCollection::action("nextTrack"), SIGNAL(triggered()), _playlistModel, SLOT(playNextTrack()));

	//Previous track
	disconnect(ActionCollection::action("previousTrack"), 0, 0, 0);
	connect(ActionCollection::action("previousTrack"), SIGNAL(triggered()), _playlistModel, SLOT(playPreviousTrack()));
}

void PlaylistView::resizeColumnsToContents() {
	static const int COLUMN_MAX_WIDTH = 300;
	static const int COLUMN_MARGIN = 20;

	//Does not start at 0 and does not finish at columnCount():
	//drops the first and last columns
	for (int i = 1; i < _playlistModel->columnCount() - 1; i++) {
		_ui->treeView->resizeColumnToContents(i);

		//Cannot be over a maximum of COLUMN_MAX_WIDTH pixels
		if (_ui->treeView->columnWidth(i) > COLUMN_MAX_WIDTH) {
			_ui->treeView->setColumnWidth(i, COLUMN_MAX_WIDTH);
		}

		//Add a margin to make it look nice
		_ui->treeView->setColumnWidth(i, _ui->treeView->columnWidth(i) + COLUMN_MARGIN);
	}
}
