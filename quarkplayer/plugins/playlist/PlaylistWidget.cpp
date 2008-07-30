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

#include "PlaylistWidget.h"

#include "PlaylistModel.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/FileExtensions.h>
#include <quarkplayer/config/Config.h>

#include <tkutil/ActionCollection.h>
#include <tkutil/TkIcon.h>
#include <tkutil/FindFiles.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>
#include <tkutil/KeyEventFilter.h>
#include <tkutil/DropEventFilter.h>

#include <playlistparser/PlaylistParser.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtGui/QtGui>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(PlaylistWidget, PlaylistWidgetFactory);

PluginInterface * PlaylistWidgetFactory::create(QuarkPlayer & quarkPlayer) const {
	return new PlaylistWidget(quarkPlayer);
}

class DragAndDropTreeView : public QTreeView {
public:

	DragAndDropTreeView() {
		setUniformRowHeights(true);
		setDragEnabled(true);
		setAcceptDrops(true);
		setDropIndicatorShown(true);
		setDragDropMode(QAbstractItemView::DragDrop);
		setRootIsDecorated(false);
		setAllColumnsShowFocus(true);
		setSelectionBehavior(QAbstractItemView::SelectRows);
		setSelectionMode(QAbstractItemView::ExtendedSelection);
		setSortingEnabled(true);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}

private:

	void mousePressEvent(QMouseEvent *event) {
		//This should be an internal move
		//since we are doing a drag&drop within the playlist, not from outside
		setDragDropMode(QAbstractItemView::InternalMove);
		QTreeView::mousePressEvent(event);
	}

	void dragEvent(QDropEvent * event) {
		if (event->source() == this) {
			//This should be an internal move
			//since we are doing a drag&drop within the playlist, not from outside
			setDragDropMode(QAbstractItemView::InternalMove);
		} else {
			setDragDropMode(QAbstractItemView::DragDrop);
		}
	}

	void dropEvent(QDropEvent * event) {
		dragEvent(event);
		QTreeView::dropEvent(event);
	}

	void dragEnterEvent(QDragEnterEvent * event) {
		dragEvent(event);
		QTreeView::dragEnterEvent(event);
	}

	void dragMoveEvent(QDragMoveEvent * event) {
		dragEvent(event);
		QTreeView::dragMoveEvent(event);
	}
};

PlaylistWidget::PlaylistWidget(QuarkPlayer & quarkPlayer)
	: PluginInterface(quarkPlayer),
	QWidget(NULL) {

	_treeView = new DragAndDropTreeView();
	QVBoxLayout * layout = new QVBoxLayout();
	setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(_treeView);

	_playlistModel = new PlaylistModel(this, quarkPlayer);
	connect(_treeView, SIGNAL(activated(const QModelIndex &)),
		_playlistModel, SLOT(play(const QModelIndex &)));

	_treeView->setModel(_playlistModel);

	//Default column sizes
	_treeView->resizeColumnToContents(PlaylistModel::COLUMN_TRACK);
	_treeView->setColumnWidth(PlaylistModel::COLUMN_TITLE, 200);
	_treeView->setColumnWidth(PlaylistModel::COLUMN_ARTIST, 150);
	_treeView->setColumnWidth(PlaylistModel::COLUMN_ALBUM, 150);
	_treeView->resizeColumnToContents(PlaylistModel::COLUMN_LENGTH);

	populateActionCollection();
	createPlaylistToolBar();

	//Add to the main window
	_dockWidget = new QDockWidget();
	quarkPlayer.mainWindow().addPlaylistDockWidget(_dockWidget);
	_dockWidget->setWidget(this);

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));
	RETRANSLATE(this);
	retranslate();
}

PlaylistWidget::~PlaylistWidget() {
}

void PlaylistWidget::createPlaylistToolBar() {
	_playlistToolBar = new QToolBar(NULL);
	_playlistToolBar->setIconSize(QSize(16, 16));
	layout()->addWidget(_playlistToolBar);

	_playlistToolBar->addAction(ActionCollection::action("playlistOpen"));
	connect(ActionCollection::action("playlistOpen"), SIGNAL(triggered()), SLOT(openPlaylist()));
	_playlistToolBar->addAction(ActionCollection::action("playlistSave"));
	connect(ActionCollection::action("playlistSave"), SIGNAL(triggered()), SLOT(savePlaylist()));
	//_playlistToolBar->addSeparator();

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
	connect(ActionCollection::action("playlistRemoveSelected"), SIGNAL(triggered()), SLOT(clearSelection()));
	KeyPressEventFilter * deleteKeyFilter = new KeyPressEventFilter(this, SLOT(clearSelection()), Qt::Key_Delete);
	_treeView->installEventFilter(deleteKeyFilter);
	removeMenu->addAction(ActionCollection::action("playlistRemoveAll"));
	connect(ActionCollection::action("playlistRemoveAll"), SIGNAL(triggered()), _playlistModel, SLOT(clear()));
	removeButton->setMenu(removeMenu);

	//_playlistToolBar->addSeparator();
	_playlistToolBar->addAction(ActionCollection::action("playlistShuffle"));
	connect(ActionCollection::action("playlistShuffle"), SIGNAL(toggled(bool)), _playlistModel, SLOT(setShuffle(bool)));
	_playlistToolBar->addAction(ActionCollection::action("playlistRepeat"));
	connect(ActionCollection::action("playlistRepeat"), SIGNAL(toggled(bool)), _playlistModel, SLOT(setRepeat(bool)));

	//Search toolbar
	//_playlistToolBar->addSeparator();
	QLineEdit * searchLineEdit = new QLineEdit();
	_playlistToolBar->addWidget(searchLineEdit);
	connect(searchLineEdit, SIGNAL(textChanged(const QString &)), SLOT(search(const QString &)));
	QToolButton * clearSearchButton = new QToolButton();
	clearSearchButton->setAutoRaise(true);
	clearSearchButton->setDefaultAction(ActionCollection::action("playlistClearSearch"));
	clearSearchButton->setEnabled(false);
	_playlistToolBar->addWidget(clearSearchButton);
	connect(clearSearchButton, SIGNAL(clicked()), searchLineEdit, SLOT(clear()));

	_playlistToolBar->addAction(ActionCollection::action("playlistNew"));
	connect(ActionCollection::action("playlistNew"), SIGNAL(triggered()), SLOT(createNewPlaylistWidget()));
}

void PlaylistWidget::populateActionCollection() {
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

	QAction * action = new QAction(app);
	action->setCheckable(true);
	ActionCollection::addAction("playlistShuffle", action);
	action = new QAction(app);
	action->setCheckable(true);
	ActionCollection::addAction("playlistRepeat", action);

	ActionCollection::addAction("playlistClearSearch", new QAction(app));

	ActionCollection::addAction("playlistNew", new QAction(app));
}

void PlaylistWidget::retranslate() {
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

	ActionCollection::action("playlistShuffle")->setText(tr("Shuffle"));
	ActionCollection::action("playlistShuffle")->setIcon(TkIcon("media-playlist-shuffle"));

	ActionCollection::action("playlistRepeat")->setText(tr("Repeat"));
	ActionCollection::action("playlistRepeat")->setIcon(TkIcon("media-playlist-repeat"));

	ActionCollection::action("playlistClearSearch")->setText(tr("Clear Search"));
	ActionCollection::action("playlistClearSearch")->setIcon(TkIcon("edit-delete"));

	ActionCollection::action("playlistNew")->setText(tr("New Playlist Window"));
	ActionCollection::action("playlistNew")->setIcon(TkIcon("preferences-system-windows"));

	_playlistToolBar->setMinimumSize(_playlistToolBar->sizeHint());

	_dockWidget->setWindowTitle(tr("Playlist"));
}

void PlaylistWidget::addFiles() {
	QStringList files = TkFileDialog::getOpenFileNames(this, tr("Select Audio/Video Files"), Config::instance().lastDirectoryUsed());
	_playlistModel->addFiles(files);
}

void PlaylistWidget::addDir() {
	QStringList files;

	QString dir = TkFileDialog::getExistingDirectory(this, tr("Select Directory"),
			Config::instance().lastDirectoryUsed());

	QStringList tmp/*(FindFiles::findAllFiles(dir))*/;
	foreach (QString file, tmp) {
		QFileInfo fileInfo(file);
		bool isMultimediaFile = FileExtensions::multimedia().contains(fileInfo.suffix(), Qt::CaseInsensitive);
		if (isMultimediaFile) {
			files << file;
		}
	}

	_playlistModel->addFiles(files);
}

void PlaylistWidget::addURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));

	QStringList files;
	files << url;

	_playlistModel->addFiles(files);
}

void PlaylistWidget::openPlaylist() {
	/*QString file = TkFileDialog::getOpenFileName(this, tr("Select Playlist File"), Config::instance().lastDirectoryUsed());
	PlaylistParser parser(file);
	connect(&parser, SIGNAL(), SLOT());
	_playlistModel->clear();
	parser.load();
	_playlistModel->addFiles();*/
}

void PlaylistWidget::savePlaylist() {
	QString file = TkFileDialog::getSaveFileName(this, tr("Save Playlist File"), Config::instance().lastDirectoryUsed());
	PlaylistParser parser(file);
	parser.save(_playlistModel->files());
}

void PlaylistWidget::clearSelection() {
	QModelIndexList list = _treeView->selectionModel()->selectedIndexes();
	QList<int> rows;
	foreach (QModelIndex index, list) {
		int row = index.row();
		if (!rows.contains(row)) {
			rows += row;
		}
	}

	if (!rows.isEmpty()) {
		_playlistModel->removeRows(rows.first(), rows.size());
	}
}

void PlaylistWidget::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	//Next track
	disconnect(ActionCollection::action("nextTrack"), 0, 0, 0);
	connect(ActionCollection::action("nextTrack"), SIGNAL(triggered()), _playlistModel, SLOT(playNextTrack()));

	//Previous track
	disconnect(ActionCollection::action("previousTrack"), 0, 0, 0);
	connect(ActionCollection::action("previousTrack"), SIGNAL(triggered()), _playlistModel, SLOT(playPreviousTrack()));
}

void PlaylistWidget::createNewPlaylistWidget() {
	new PlaylistWidget(quarkPlayer());
}
