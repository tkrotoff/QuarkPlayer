/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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
#include <quarkplayer/config/Config.h>
#include <quarkplayer/PluginManager.h>

#include <quarkplayer-plugins/MainWindow/MainWindow.h>
#include <quarkplayer-plugins/ConfigWindow/ConfigWindowPlugin.h>
#include <quarkplayer-plugins/ConfigWindow/PlaylistConfig.h>

#include <TkUtil/TkIcon.h>
#include <TkUtil/TkAction.h>
#include <TkUtil/ActionCollection.h>
#include <TkUtil/TkFileDialog.h>
#include <TkUtil/LanguageChangeEventFilter.h>
#include <TkUtil/KeyEventFilter.h>
#include <TkUtil/SearchLineEdit.h>

#include <FileTypes/FileTypes.h>

#include <PlaylistParser/PlaylistParser.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

static const char * PLAYLIST_SEARCH_HISTORY_KEY = "playlist_search_history";

Q_EXPORT_PLUGIN2(Playlist, PlaylistWidgetFactory);

const char * PlaylistWidgetFactory::PLUGIN_NAME = "Playlist";

QStringList PlaylistWidgetFactory::dependencies() const {
	QStringList tmp;
	tmp += MainWindowFactory::PLUGIN_NAME;
	tmp += ConfigWindowPluginFactory::PLUGIN_NAME;
	return tmp;
}

PluginInterface * PlaylistWidgetFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new PlaylistWidget(quarkPlayer, uuid);
}

PlaylistWidget * PlaylistWidgetFactory::playlistWidget() {
	PlaylistWidget * playlistWidget = dynamic_cast<PlaylistWidget *>(PluginManager::instance().pluginInterface(PLUGIN_NAME));
	//Q_ASSERT(playlistWidget);
	return playlistWidget;
}

PlaylistWidget::PlaylistWidget(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QWidget(MainWindowFactory::mainWindow()),
	PluginInterface(quarkPlayer, uuid) {

	//Model
	_playlistModel = new PlaylistModel(this, quarkPlayer, uuid);
	connect(_playlistModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
		SLOT(updateWindowTitle()));
	connect(_playlistModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
		SLOT(updateWindowTitle()));
	connect(_playlistModel, SIGNAL(playlistLoaded(PlaylistParser::Error, int)),
		SLOT(playlistLoaded(PlaylistParser::Error, int)));
	connect(_playlistModel, SIGNAL(playlistSaved(PlaylistParser::Error, int)),
		SLOT(playlistSaved(PlaylistParser::Error, int)));
	///

	//Filter
	_playlistFilter = new PlaylistFilter(_playlistModel);
	///

	_playlistModel->setPlaylistFilter(_playlistFilter);

	//TreeView
	_treeView = new DragAndDropTreeView(this);
	connect(_treeView, SIGNAL(activated(const QModelIndex &)),
		_playlistFilter, SLOT(play(const QModelIndex &)));
	_treeView->setModel(_playlistFilter);
	QVBoxLayout * layout = new QVBoxLayout();
	setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);
	///

	//Default column sizes
	_treeView->setColumnWidth(PlaylistModel::COLUMN_INFO, 18);
	//_treeView->resizeColumnToContents(PlaylistModel::COLUMN_INFO);
	_treeView->resizeColumnToContents(PlaylistModel::COLUMN_TRACK);
	_treeView->setColumnWidth(PlaylistModel::COLUMN_TITLE, 200);
	_treeView->setColumnWidth(PlaylistModel::COLUMN_ARTIST, 150);
	_treeView->setColumnWidth(PlaylistModel::COLUMN_ALBUM, 150);
	_treeView->resizeColumnToContents(PlaylistModel::COLUMN_LENGTH);
	///

	populateActionCollection();
	createToolBar();

	layout->addWidget(_treeView);

	//Add to the main window
	_dockWidget = new QDockWidget();
	connect(_dockWidget, SIGNAL(visibilityChanged(bool)),
		SLOT(dockWidgetVisibilityChanged(bool)));
	MainWindowFactory::mainWindow()->addPlaylistDockWidget(_dockWidget);
	_dockWidget->setWidget(this);

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	//Determine if this playlist is the active one or not
	connect(&PlaylistConfig::instance(), SIGNAL(activePlaylistChanged(const QUuid &)),
		SLOT(activePlaylistChanged(const QUuid &)));

	Config::instance().addKey(PLAYLIST_SEARCH_HISTORY_KEY, QStringList());

	RETRANSLATE(this);
	retranslate();
}

PlaylistWidget::~PlaylistWidget() {
	MainWindowFactory::mainWindow()->removeDockWidget(_dockWidget);
	MainWindowFactory::mainWindow()->resetPlaylistDockWidget();
}

PlaylistModel * PlaylistWidget::playlistModel() const {
	return _playlistModel;
}

PlaylistFilter * PlaylistWidget::playlistFilter() const {
	return _playlistFilter;
}

void PlaylistWidget::createToolBar() {
	QToolBar * toolBar = new QToolBar(NULL);
	toolBar->setIconSize(QSize(16, 16));
	layout()->addWidget(toolBar);

	//Needed by addWordToWordList()
	_searchTimer = new QTimer(this);
	_searchTimer->setSingleShot(true);
	_searchTimer->setInterval(1500);
	connect(_searchTimer, SIGNAL(timeout()), SLOT(addWordToWordList()));

	toolBar->addAction(uuidAction("Playlist.Shuffle"));
	connect(uuidAction("Playlist.Shuffle"), SIGNAL(toggled(bool)), _playlistFilter, SLOT(setShuffle(bool)));
	toolBar->addAction(uuidAction("Playlist.Repeat"));
	connect(uuidAction("Playlist.Repeat"), SIGNAL(toggled(bool)), _playlistFilter, SLOT(setRepeat(bool)));

	toolBar->addAction(uuidAction("Playlist.JumpToCurrent"));
	connect(uuidAction("Playlist.JumpToCurrent"), SIGNAL(triggered()), SLOT(jumpToCurrent()));

	//Search line edit
	QStringList history = Config::instance().value(PLAYLIST_SEARCH_HISTORY_KEY).toStringList();
	_searchLineEdit = new SearchLineEdit(history, toolBar);
	connect(_searchLineEdit, SIGNAL(textChanged(const QString &)), SLOT(search()));
	toolBar->addWidget(_searchLineEdit);

	toolBar->addAction(uuidAction("Playlist.Open"));
	connect(uuidAction("Playlist.Open"), SIGNAL(triggered()), SLOT(openPlaylist()));
	toolBar->addAction(uuidAction("Playlist.Save"));
	connect(uuidAction("Playlist.Save"), SIGNAL(triggered()), SLOT(savePlaylist()));

	//We have to use a QToolButton instead of a QAction,
	//otherwise we cannot use QToolButton::InstantPopup :/
	QToolButton * addButton = new QToolButton();
	addButton->setPopupMode(QToolButton::InstantPopup);
	addButton->setDefaultAction(uuidAction("Playlist.Add"));
	toolBar->addWidget(addButton);

	QMenu * addMenu = new QMenu();
	addMenu->addAction(uuidAction("Playlist.AddFiles"));
	connect(uuidAction("Playlist.AddFiles"), SIGNAL(triggered()), SLOT(addFiles()));
	addMenu->addAction(uuidAction("Playlist.AddDirectory"));
	connect(uuidAction("Playlist.AddDirectory"), SIGNAL(triggered()), SLOT(addDir()));
	addMenu->addAction(uuidAction("Playlist.AddURL"));
	connect(uuidAction("Playlist.AddURL"), SIGNAL(triggered()), SLOT(addURL()));
	addButton->setMenu(addMenu);

	KeyPressEventFilter * deleteKeyFilter = new KeyPressEventFilter(_treeView, SLOT(clearSelection()), Qt::Key_Delete);
	_treeView->installEventFilter(deleteKeyFilter);

	toolBar->addAction(uuidAction("Playlist.RemoveAll"));
	connect(uuidAction("Playlist.RemoveAll"), SIGNAL(triggered()), _playlistModel, SLOT(clear()));
	connect(uuidAction("Playlist.RemoveAll"), SIGNAL(triggered()), SLOT(updateWindowTitle()));

	toolBar->addSeparator();

	toolBar->addAction(uuidAction("Playlist.New"));
	connect(uuidAction("Playlist.New"), SIGNAL(triggered()), SLOT(createNewPlaylistWidget()));
}

void PlaylistWidget::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	addUuidAction("Playlist.Open", new QAction(app));
	addUuidAction("Playlist.Save", new QAction(app));

	addUuidAction("Playlist.Add", new QAction(app));
	addUuidAction("Playlist.AddFiles", new QAction(app));
	addUuidAction("Playlist.AddDirectory", new QAction(app));
	addUuidAction("Playlist.AddURL", new QAction(app));

	addUuidAction("Playlist.RemoveAll", new QAction(app));

	TkAction * action = new TkAction(app, tr("Ctrl+S"));
	action->setCheckable(true);
	addUuidAction("Playlist.Shuffle", action);

	action = new TkAction(app, tr("Ctrl+R"));
	action->setCheckable(true);
	addUuidAction("Playlist.Repeat", action);

	addUuidAction("Playlist.JumpToCurrent", new TkAction(app, tr("Ctrl+J")));

	addUuidAction("Playlist.New", new QAction(app));
}

void PlaylistWidget::retranslate() {
	_searchLineEdit->clearButton()->setToolTip(tr("Clear Search"));
	_searchLineEdit->clearButton()->setIcon(TkIcon("edit-clear-locationbar-rtl"));

	_searchLineEdit->showWordListButton()->setToolTip(tr("Previous Searches"));
	_searchLineEdit->showWordListButton()->setIcon(TkIcon("go-down-search"));

	_searchLineEdit->setToolTip(tr("Search the playlist, use whitespaces to separate words"));
	_searchLineEdit->setClickMessage(tr("Search"));

	uuidAction("Playlist.Open")->setText(tr("Open Playlist"));
	uuidAction("Playlist.Open")->setIcon(TkIcon("document-open"));

	uuidAction("Playlist.Save")->setText(tr("Save Playlist"));
	uuidAction("Playlist.Save")->setIcon(TkIcon("document-save"));

	uuidAction("Playlist.Add")->setText(tr("Add..."));
	uuidAction("Playlist.Add")->setIcon(TkIcon("list-add"));

	uuidAction("Playlist.AddFiles")->setText(tr("Add Files"));
	uuidAction("Playlist.AddDirectory")->setText(tr("Add Directory"));
	uuidAction("Playlist.AddURL")->setText(tr("Add URL"));

	uuidAction("Playlist.RemoveAll")->setText(tr("Remove All"));
	uuidAction("Playlist.RemoveAll")->setIcon(TkIcon("list-remove"));

	uuidAction("Playlist.Shuffle")->setText(tr("Shuffle"));
	uuidAction("Playlist.Shuffle")->setIcon(TkIcon("media-playlist-shuffle"));

	uuidAction("Playlist.Repeat")->setText(tr("Repeat"));
	uuidAction("Playlist.Repeat")->setIcon(TkIcon("media-playlist-repeat"));

	uuidAction("Playlist.JumpToCurrent")->setText(tr("Jump to Current Playing Media"));
	uuidAction("Playlist.JumpToCurrent")->setIcon(TkIcon("go-jump"));

	uuidAction("Playlist.New")->setText(tr("New Playlist Window"));
	uuidAction("Playlist.New")->setIcon(TkIcon("window-new"));

	updateWindowTitle();
}

void PlaylistWidget::addFiles() {
	QStringList files = TkFileDialog::getOpenFileNames(
		this, tr("Select Audio/Video Files"), Config::instance().lastDirOpened(),
		tr("Multimedia") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video, FileType::Audio)) + ";;" +
		tr("Video") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video)) +";;" +
		tr("Audio") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Audio)) +";;" +
		tr("Playlist") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Playlist)) + ";;" +
		tr("All Files") + " (*.*)"
	);

	if (!files.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIR_OPENED_KEY, QFileInfo(files[0]).absolutePath());

		addFilesToCurrentPlaylist(files);
	}
}

void PlaylistWidget::addFilesToCurrentPlaylist(const QStringList & files) {
	if (!files.isEmpty()) {
		if (uuid() == PlaylistConfig::instance().activePlaylist()) {
			_playlistModel->addFiles(files);
			_playlistModel->saveCurrentPlaylist();
		}
	}
}

void PlaylistWidget::addDir() {
	QStringList files;

	QString dir = TkFileDialog::getExistingDirectory(this, tr("Select Directory"),
			Config::instance().lastDirOpened());
	if (!dir.isEmpty()) {
		QStringList tmp;
		tmp << dir;
		_playlistModel->addFiles(tmp);
	}
}

void PlaylistWidget::addURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));
	if (!url.isEmpty()) {
		QStringList tmp;
		tmp << url;
		_playlistModel->addFiles(tmp);
		_playlistModel->saveCurrentPlaylist();
	}
}

void PlaylistWidget::openPlaylist() {
	QString filename = TkFileDialog::getOpenFileName(
		this, tr("Select Playlist File"), Config::instance().lastDirOpened(),
		tr("Playlist") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Playlist)) + ";;" +
		tr("All Files") + " (*.*)"
	);

	if (!filename.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIR_OPENED_KEY, QFileInfo(filename).absolutePath());
		_playlistModel->clear();
		_playlistModel->loadPlaylist(filename);
	}
}

void PlaylistWidget::playlistLoaded(PlaylistParser::Error, int timeElapsed) {
	updateWindowTitle(tr("Playlist loaded:") + ' ' + QString::number((float) timeElapsed / 1000) + ' ' + tr("seconds") +
			" (" + QString::number(_playlistModel->rowCount()) + ' ' + tr("medias") + ')');
}

void PlaylistWidget::playlistSaved(PlaylistParser::Error, int timeElapsed) {
	updateWindowTitle(tr("Playlist saved:") + ' ' + QString::number((float) timeElapsed / 1000) + ' ' + tr("seconds") +
			" (" + QString::number(_playlistModel->rowCount()) + ' ' + tr("medias") + ')');
}

void PlaylistWidget::updateWindowTitle(const QString & statusMessage) {
	QString nbMedias(QString::number(_playlistModel->rowCount()));
	QString windowTitle(nbMedias + ' ' + tr("medias"));
	if (!statusMessage.isEmpty()) {
		windowTitle += " - " + statusMessage;
	}
	_dockWidget->setWindowTitle(windowTitle);
	QStatusBar * statusBar = MainWindowFactory::mainWindow()->statusBar();
	if (statusBar) {
		statusBar->showMessage(statusMessage);
	}
}

void PlaylistWidget::savePlaylist() {
	static const char * PLAYLIST_DEFAULT_EXTENSION = "m3u8";

	QString filename = TkFileDialog::getSaveFileName(
		this, tr("Save Playlist File"), Config::instance().lastDirOpened(),
		FileTypes::toSaveFilterFormat(FileTypes::extensions(FileType::Playlist), PLAYLIST_DEFAULT_EXTENSION) +
		tr("All Files") + " (*.*)"
	);

	if (!filename.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIR_OPENED_KEY, QFileInfo(filename).absolutePath());

		static PlaylistWriter * parser = new PlaylistWriter(this);
		connect(parser, SIGNAL(finished(PlaylistParser::Error, int)),
			SLOT(playlistSaved(PlaylistParser::Error, int)));
		parser->save(filename, _playlistModel->files());
	}
}

void PlaylistWidget::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	disconnectFromMediaObjectList();
	if (PlaylistConfig::instance().activePlaylist() == uuid()) {
		connectToMediaObject(mediaObject);
	}
}

void PlaylistWidget::connectToMediaObject(Phonon::MediaObject * mediaObject) {
	if (!mediaObject) {
		return;
	}

	//Next track
	connect(ActionCollection::action("MainWindow.NextTrack"), SIGNAL(triggered()),
		_playlistFilter, SLOT(playNextTrack()));

	//Previous track
	connect(ActionCollection::action("MainWindow.PreviousTrack"), SIGNAL(triggered()),
		_playlistFilter, SLOT(playPreviousTrack()));
}

void PlaylistWidget::disconnectFromMediaObjectList() {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
		tmp->disconnect(_playlistFilter);
	}

	//Next track
	ActionCollection::action("MainWindow.NextTrack")->disconnect(_playlistFilter);

	//Previous track
	ActionCollection::action("MainWindow.PreviousTrack")->disconnect(_playlistFilter);
}

void PlaylistWidget::createNewPlaylistWidget() {
	PluginManager::instance().loadDisabledPlugin(PlaylistWidgetFactory::PLUGIN_NAME);
}

void PlaylistWidget::jumpToCurrent() {
	QModelIndex index = _playlistFilter->currentIndex();
	if (!_playlistFilter->mapFromSource(index).isValid()) {
		//_searchLineEdit->clear();
		search();
	}
	_treeView->scrollTo(_playlistFilter->mapFromSource(index), QAbstractItemView::PositionAtCenter);
}

void PlaylistWidget::search() {
	QTime timeElapsed;
	timeElapsed.start();

	QString pattern(_searchLineEdit->text());
	if (!pattern.isEmpty()) {
		updateWindowTitle(tr("Searching..."));
	} else {
		updateWindowTitle();
	}

	QString tmp;
	QStringList words(pattern.split(' '));
	for (int i = 0; i < words.size(); i++) {
		tmp += '(' + words[i] + ')';
		if (i < words.size() - 1) {
			tmp += ".*";
		}
	}
	qDebug() << __FUNCTION__ << tmp;

	_playlistFilter->setFilterRegExp(QRegExp(tmp,
			Qt::CaseInsensitive, QRegExp::RegExp2));

	//Force the treeView to launch files tags fetching
	//FIXME Does not work
	//_treeView->repaint();
	///

	if (!pattern.isEmpty()) {
		updateWindowTitle(tr("Search finished:") + ' ' + QString::number((float) timeElapsed.elapsed() / 1000) + ' ' + tr("seconds") +
			" (" + QString::number(_playlistFilter->rowCount()) + ' ' + tr("medias") + ')');
	} else {
		updateWindowTitle();
	}

	//Needed by addWordToWordList()
	_searchTimer->stop();
	_searchTimer->start();
}

void PlaylistWidget::addWordToWordList() {
	_searchTimer->stop();

	//Add the word searched inside the SearchLineEdit
	QString word = _searchLineEdit->text();
	qDebug() << __FUNCTION__ << "Word:" << word;
	if (word.size() > 2) {
		_searchLineEdit->addWord(word);
		QStringList wordList = _searchLineEdit->wordList();
		QStringList tmp;
		//Only takes the first 100 words from the list and save them
		for (int i = 0; i < wordList.size() && i < 100; i++) {
			tmp += wordList[i];
		}
		Config::instance().setValue(PLAYLIST_SEARCH_HISTORY_KEY, tmp);
	}
}

bool PlaylistWidget::event(QEvent * event) {
	switch (event->type()) {
	case QEvent::WindowActivate:
		//When this window is active, this playlist becomes the only active one
		PlaylistConfig::instance().setActivePlaylist(uuid());
		break;
	default:
		break;
	}
	return QWidget::event(event);
}

void PlaylistWidget::dockWidgetVisibilityChanged(bool visible) {
	if (visible) {
		//When this dockwidget is visible, this playlist becomes the only active one
		PlaylistConfig::instance().setActivePlaylist(uuid());
	}
}

void PlaylistWidget::activePlaylistChanged(const QUuid & _uuid) {
	if (uuid() == _uuid) {
		//This playlist is the active one

		disconnectFromMediaObjectList();
		connectToMediaObject(quarkPlayer().currentMediaObject());

		QString title = _dockWidget->windowTitle();
		if (title[0] != '!') {
			_dockWidget->setWindowTitle('!' + title);
		}
	} else {
		//This playlist is not the active one

		disconnectFromMediaObjectList();

		//updateWindowTitle();
		QString title = _dockWidget->windowTitle();
		if (title[0] == '!') {
			title.remove(0, 1);
			_dockWidget->setWindowTitle(title);
		}
	}
}