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

#include "DragAndDropTreeView.h"
#include "PlaylistModel.h"
#include "PlaylistFilter.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/config/Config.h>

#include <tkutil/ActionCollection.h>
#include <tkutil/TkIcon.h>
#include <tkutil/FindFiles.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>
#include <tkutil/KeyEventFilter.h>

#include <filetypes/FileTypes.h>

#include <playlistparser/PlaylistParser.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtGui/QtGui>

#include <QtCore/QtConcurrentRun>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

Q_EXPORT_PLUGIN2(PlaylistWidget, PlaylistWidgetFactory);

PluginInterface * PlaylistWidgetFactory::create(QuarkPlayer & quarkPlayer) const {
	return new PlaylistWidget(quarkPlayer);
}

PlaylistWidget::PlaylistWidget(QuarkPlayer & quarkPlayer)
	: QWidget(NULL),
	PluginInterface(quarkPlayer) {

	//Model
	_playlistModel = new PlaylistModel(this, quarkPlayer);
	connect(_playlistModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
		SLOT(updateWindowTitle()));
	connect(_playlistModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
		SLOT(updateWindowTitle()));

	//Filter
	_playlistFilter = new PlaylistFilter(this, _playlistModel);

	//TreeView
	_treeView = new DragAndDropTreeView(_playlistModel, _playlistFilter);
	connect(_treeView, SIGNAL(activated(const QModelIndex &)),
		_playlistFilter, SLOT(play(const QModelIndex &)));
	_treeView->setModel(_playlistFilter);
	QVBoxLayout * layout = new QVBoxLayout();
	setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(_treeView);

	//Default column sizes
	_treeView->resizeColumnToContents(PlaylistModel::COLUMN_TRACK);
	_treeView->setColumnWidth(PlaylistModel::COLUMN_TITLE, 200);
	_treeView->setColumnWidth(PlaylistModel::COLUMN_ARTIST, 150);
	_treeView->setColumnWidth(PlaylistModel::COLUMN_ALBUM, 150);
	_treeView->resizeColumnToContents(PlaylistModel::COLUMN_LENGTH);

	populateActionCollection();
	createToolBar();

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

void PlaylistWidget::createToolBar() {
	_toolBar = new QToolBar(NULL);
	_toolBar->setIconSize(QSize(16, 16));
	layout()->addWidget(_toolBar);

	_toolBar->addAction(ActionCollection::action("playlistOpen"));
	connect(ActionCollection::action("playlistOpen"), SIGNAL(triggered()), SLOT(openPlaylist()));
	_toolBar->addAction(ActionCollection::action("playlistSave"));
	connect(ActionCollection::action("playlistSave"), SIGNAL(triggered()), SLOT(savePlaylist()));

	//We have to use a QToolButton instead of a QAction,
	//otherwise we cannot use QToolButton::InstantPopup :/
	QToolButton * addButton = new QToolButton();
	addButton->setPopupMode(QToolButton::InstantPopup);
	addButton->setDefaultAction(ActionCollection::action("playlistAdd"));
	_toolBar->addWidget(addButton);

	QToolButton * removeButton = new QToolButton();
	removeButton->setPopupMode(QToolButton::InstantPopup);
	removeButton->setDefaultAction(ActionCollection::action("playlistRemove"));
	_toolBar->addWidget(removeButton);

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
	connect(ActionCollection::action("playlistRemoveSelected"), SIGNAL(triggered()), _treeView, SLOT(clearSelection()));
	KeyPressEventFilter * deleteKeyFilter = new KeyPressEventFilter(_treeView, SLOT(clearSelection()), Qt::Key_Delete);
	_treeView->installEventFilter(deleteKeyFilter);
	removeMenu->addAction(ActionCollection::action("playlistRemoveAll"));
	connect(ActionCollection::action("playlistRemoveAll"), SIGNAL(triggered()), _playlistModel, SLOT(clear()));
	connect(ActionCollection::action("playlistRemoveAll"), SIGNAL(triggered()), SLOT(updateWindowTitle()));
	removeButton->setMenu(removeMenu);

	_toolBar->addAction(ActionCollection::action("playlistShuffle"));
	connect(ActionCollection::action("playlistShuffle"), SIGNAL(toggled(bool)), _playlistFilter, SLOT(setShuffle(bool)));
	_toolBar->addAction(ActionCollection::action("playlistRepeat"));
	connect(ActionCollection::action("playlistRepeat"), SIGNAL(toggled(bool)), _playlistFilter, SLOT(setRepeat(bool)));

	_toolBar->addAction(ActionCollection::action("playlistJumpToCurrent"));
	connect(ActionCollection::action("playlistJumpToCurrent"), SIGNAL(triggered()), SLOT(jumpToCurrent()));

	//Search toolbar
	_searchLineEdit = new QLineEdit();
	_toolBar->addWidget(_searchLineEdit);
	_searchTimer = new QTimer(this);
	_searchTimer->setSingleShot(true);
	_searchTimer->setInterval(700);
	connect(_searchTimer, SIGNAL(timeout()), SLOT(search()));
	connect(_searchLineEdit, SIGNAL(returnPressed()), SLOT(search()));
	connect(_searchLineEdit, SIGNAL(textChanged(const QString &)), SLOT(searchChanged()));
	_clearSearchButton = new QToolButton();
	_clearSearchButton->setAutoRaise(true);
	_clearSearchButton->setDefaultAction(ActionCollection::action("playlistClearSearch"));
	_toolBar->addWidget(_clearSearchButton);
	connect(_clearSearchButton, SIGNAL(clicked()), _searchLineEdit, SLOT(clear()));

	_toolBar->addAction(ActionCollection::action("playlistNew"));
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

	ActionCollection::addAction("playlistJumpToCurrent", new QAction(app));

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

	ActionCollection::action("playlistJumpToCurrent")->setText(tr("Jump to Current Playing Media"));
	ActionCollection::action("playlistJumpToCurrent")->setIcon(TkIcon("go-jump"));

	ActionCollection::action("playlistClearSearch")->setText(tr("Clear Search"));
	ActionCollection::action("playlistClearSearch")->setIcon(TkIcon("edit-delete"));

	_searchLineEdit->setToolTip(tr("Search Playlist"));
	QString pattern(_searchLineEdit->text().trimmed());
	_clearSearchButton->setEnabled(!pattern.isEmpty());

	ActionCollection::action("playlistNew")->setText(tr("New Playlist Window"));
	ActionCollection::action("playlistNew")->setIcon(TkIcon("preferences-system-windows"));

	_toolBar->setMinimumSize(_toolBar->sizeHint());
	updateWindowTitle();
}

void PlaylistWidget::updateWindowTitle() {
	QString nbItems(QString::number(_playlistModel->rowCount()));
	_dockWidget->setWindowTitle(tr("Playlist") + " - " + nbItems + " " + tr("items"));
}

void PlaylistWidget::addFiles() {
	QStringList files = TkFileDialog::getOpenFileNames(
		this, tr("Select Audio/Video Files"), Config::instance().lastDirectoryUsed(),
		tr("Multimedia") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video, FileType::Audio)) + ";;" +
		tr("Video") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video)) +";;" +
		tr("Audio") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Audio)) +";;" +
		tr("Playlist") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Playlist)) + ";;" +
		tr("All Files") + " (*.*)"
	);

	if (!files.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIRECTORY_USED_KEY, QFileInfo(files[0]).absolutePath());

		_playlistModel->addFiles(files);
		_playlistModel->saveCurrentPlaylist();
	}
}

void PlaylistWidget::addDir() {
	QStringList files;

	QString dir = TkFileDialog::getExistingDirectory(this, tr("Select Directory"),
			Config::instance().lastDirectoryUsed());

	QStringList tmp/*(FindFiles::findAllFiles(dir))*/;
	foreach (QString file, tmp) {
		QFileInfo fileInfo(file);
		bool isMultimediaFile = FileTypes::extensions(FileType::Video, FileType::Audio).contains(fileInfo.suffix(), Qt::CaseInsensitive);
		if (isMultimediaFile) {
			files << file;
		}
	}

	_playlistModel->addFiles(files);
	_playlistModel->saveCurrentPlaylist();
}

void PlaylistWidget::addURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));
	if (!url.isEmpty()) {
		QStringList files;
		files << url;

		_playlistModel->addFiles(files);
		_playlistModel->saveCurrentPlaylist();
	}
}

void PlaylistWidget::openPlaylist() {
	QString filename = TkFileDialog::getOpenFileName(
		this, tr("Select Playlist File"), Config::instance().lastDirectoryUsed(),
		tr("Playlist") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Playlist)) + ";;" +
		tr("All Files") + " (*.*)"
	);

	if (!filename.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIRECTORY_USED_KEY, QFileInfo(filename).absolutePath());

		PlaylistParser parser(filename);
		connect(&parser, SIGNAL(filesFound(const QStringList &)),
			SLOT(parserFilesFound(const QStringList &)));
		connect(&parser, SIGNAL(finished()),
			_playlistModel, SLOT(saveCurrentPlaylist()));
		_playlistModel->clear();
		parser.load();
	}
}

void PlaylistWidget::parserFilesFound(const QStringList & files) {
	_playlistModel->addFiles(files);
	QCoreApplication::processEvents();
}

void PlaylistWidget::savePlaylist() {
	static const QString defaultFileExtension("m3u");

	QString filename = TkFileDialog::getSaveFileName(
		this, tr("Save Playlist File"), Config::instance().lastDirectoryUsed(),
		FileTypes::toSaveFilterFormat(FileTypes::extensions(FileType::Playlist), defaultFileExtension) +
		tr("All Files") + " (*.*)"
	);

	if (!filename.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIRECTORY_USED_KEY, QFileInfo(filename).absolutePath());

		PlaylistParser parser(filename);
		parser.save(_playlistModel->filenames());
	}
}

void PlaylistWidget::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	//Next track
	disconnect(ActionCollection::action("nextTrack"), 0, 0, 0);
	connect(ActionCollection::action("nextTrack"), SIGNAL(triggered()),
		_playlistFilter, SLOT(playNextTrack()));

	//Previous track
	disconnect(ActionCollection::action("previousTrack"), 0, 0, 0);
	connect(ActionCollection::action("previousTrack"), SIGNAL(triggered()),
		_playlistFilter, SLOT(playPreviousTrack()));

	//aboutToFinish -> let's queue/play the next track
	connect(mediaObject, SIGNAL(aboutToFinish()),
		_playlistFilter, SLOT(enqueueNextTrack()));
}

void PlaylistWidget::createNewPlaylistWidget() {
	new PlaylistWidget(quarkPlayer());
}

void PlaylistWidget::jumpToCurrent() {
	QModelIndex index = _playlistFilter->currentIndex();
	if (!_playlistFilter->mapFromSource(index).isValid()) {
		_searchLineEdit->clear();
		search();
	}
	_treeView->scrollTo(_playlistFilter->mapFromSource(index), QAbstractItemView::PositionAtCenter);
}

void PlaylistWidget::searchChanged() {
	_searchTimer->stop();
	_searchTimer->start();
}

void PlaylistWidget::search() {
	_searchTimer->stop();
	QString pattern(_searchLineEdit->text().trimmed());
	_clearSearchButton->setEnabled(!pattern.isEmpty());

	QFutureWatcher<void> * watcher = new QFutureWatcher<void>(this);
	connect(watcher, SIGNAL(started()), SLOT(searchStarted()));
	connect(watcher, SIGNAL(finished()), SLOT(searchFinished()));
	qDebug() << __FUNCTION__ << "1";
	QFuture<void> future = QtConcurrent::run(_playlistFilter, &PlaylistFilter::setFilter, pattern);
	qDebug() << __FUNCTION__ << "2";
	watcher->setFuture(future);
}

void PlaylistWidget::searchStarted() {
	QStatusBar * statusBar = quarkPlayer().mainWindow().statusBar();
	qDebug() << __FUNCTION__ << statusBar;
	if (statusBar) {
		statusBar->showMessage(tr("Searching..."));
	}
}

void PlaylistWidget::searchFinished() {
	QStatusBar * statusBar = quarkPlayer().mainWindow().statusBar();
	qDebug() << __FUNCTION__ << statusBar;
	if (statusBar) {
		statusBar->showMessage(tr("Search finished"));
	}
}
