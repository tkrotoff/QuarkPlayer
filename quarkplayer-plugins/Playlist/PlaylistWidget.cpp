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

#include "PlaylistWidget.h"

#include "DragAndDropTreeView.h"
#include "PlaylistModel.h"
#include "PlaylistFilter.h"
#include "PlaylistLogger.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/PluginManager.h>

#include <quarkplayer-plugins/MainWindow/MainWindow.h>
#include <quarkplayer-plugins/ConfigWindow/ConfigWindowPlugin.h>
#include <quarkplayer-plugins/ConfigWindow/PlaylistConfig.h>

#include <TkUtil/TkAction.h>
#include <TkUtil/TkToolBar.h>
#include <TkUtil/TkFileDialog.h>
#include <TkUtil/LanguageChangeEventFilter.h>
#include <TkUtil/KeyEventFilter.h>
#include <TkUtil/SearchLineEdit.h>

#include <FileTypes/FileTypes.h>

#include <PlaylistParser/PlaylistParser.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtGui/QtGui>

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
	return new PlaylistWidget(quarkPlayer, uuid, MainWindowFactory::mainWindow());
}

PlaylistWidget * PlaylistWidgetFactory::playlistWidget() {
	PlaylistWidget * playlistWidget = dynamic_cast<PlaylistWidget *>(PluginManager::instance().pluginInterface(PLUGIN_NAME));
	Q_ASSERT(playlistWidget);
	return playlistWidget;
}

PlaylistWidget::PlaylistWidget(QuarkPlayer & quarkPlayer, const QUuid & uuid, IMainWindow * mainWindow)
	: QWidget(mainWindow),
	PluginInterface(quarkPlayer, uuid) {

	Q_ASSERT(mainWindow);
	_mainWindow = mainWindow;
	connect(_mainWindow, SIGNAL(filesOpened(const QStringList &)),
		SLOT(filesOpenedByMainWindow(const QStringList &)));

	//Model
	_playlistModel = new PlaylistModel(quarkPlayer, uuid, this);
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
	_view = new DragAndDropTreeView(this);
	connect(_view, SIGNAL(activated(const QModelIndex &)),
		_playlistFilter, SLOT(play(const QModelIndex &)));
	_view->setModel(_playlistFilter);
	QVBoxLayout * layout = new QVBoxLayout();
	setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);
	///

	//Default column sizes
	_view->setColumnWidth(PlaylistModel::COLUMN_INFO, 18);
	//_view->resizeColumnToContents(PlaylistModel::COLUMN_INFO);
	_view->resizeColumnToContents(PlaylistModel::COLUMN_TRACK);
	_view->setColumnWidth(PlaylistModel::COLUMN_TITLE, 200);
	_view->setColumnWidth(PlaylistModel::COLUMN_ARTIST, 150);
	_view->setColumnWidth(PlaylistModel::COLUMN_ALBUM, 150);
	_view->resizeColumnToContents(PlaylistModel::COLUMN_LENGTH);
	///

	populateActionCollection();
	createToolBar();

	layout->addWidget(_view);

	//Add to the main window
	_dockWidget = new QDockWidget();
	connect(_dockWidget, SIGNAL(visibilityChanged(bool)),
		SLOT(dockWidgetVisibilityChanged(bool)));
	_mainWindow->addPlaylistDockWidget(_dockWidget);
	_dockWidget->setWidget(this);

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	//Determine if this playlist is the active one or not
	connect(&PlaylistConfig::instance(), SIGNAL(activePlaylistChanged(const QUuid &)),
		SLOT(activePlaylistChanged(const QUuid &)));

	Config & config = Config::instance();
	if (!config.contains(PLAYLIST_SEARCH_HISTORY_KEY)) {
		config.addKey(PLAYLIST_SEARCH_HISTORY_KEY, QStringList());
	}

	RETRANSLATE(this);
	retranslate();
}

PlaylistWidget::~PlaylistWidget() {
	_mainWindow->removeDockWidget(_dockWidget);
}

PlaylistModel * PlaylistWidget::playlistModel() const {
	return _playlistModel;
}

PlaylistFilter * PlaylistWidget::playlistFilter() const {
	return _playlistFilter;
}

void PlaylistWidget::createToolBar() {
	TkToolBar * toolBar = new TkToolBar(NULL);
	toolBar->setIconSize(QSize(16, 16));
	layout()->addWidget(toolBar);

	//Needed by addWordToWordList()
	_searchTimer = new QTimer(this);
	_searchTimer->setSingleShot(true);
	_searchTimer->setInterval(1500);
	connect(_searchTimer, SIGNAL(timeout()), SLOT(addWordToWordList()));

	toolBar->addAction(_actions["Playlist.Shuffle"]);
	connect(_actions["Playlist.Shuffle"], SIGNAL(toggled(bool)), _playlistFilter, SLOT(setShuffle(bool)));

	toolBar->addAction(_actions["Playlist.Repeat"]);
	connect(_actions["Playlist.Repeat"], SIGNAL(toggled(bool)), _playlistFilter, SLOT(setRepeat(bool)));

	toolBar->addAction(_actions["Playlist.JumpToCurrent"]);
	connect(_actions["Playlist.JumpToCurrent"], SIGNAL(triggered()), SLOT(jumpToCurrent()));

	//Search line edit
	QStringList history = Config::instance().value(PLAYLIST_SEARCH_HISTORY_KEY).toStringList();
	_searchLineEdit = new SearchLineEdit(history, toolBar);
	connect(_searchLineEdit, SIGNAL(textChanged(const QString &)), SLOT(search()));
	toolBar->addWidget(_searchLineEdit);

	toolBar->addAction(_actions["Playlist.Open"]);
	connect(_actions["Playlist.Open"], SIGNAL(triggered()), SLOT(openPlaylist()));

	toolBar->addAction(_actions["Playlist.Save"]);
	connect(_actions["Playlist.Save"], SIGNAL(triggered()), SLOT(savePlaylist()));

	//We have to use a QToolButton instead of a QAction,
	//otherwise we cannot use QToolButton::InstantPopup :/
	QToolButton * addButton = new QToolButton();
	addButton->setPopupMode(QToolButton::InstantPopup);
	addButton->setDefaultAction(_actions["Playlist.Add"]);
	toolBar->addWidget(addButton);

	QMenu * addMenu = new QMenu();
	addMenu->addAction(_actions["Playlist.AddFiles"]);
	connect(_actions["Playlist.AddFiles"], SIGNAL(triggered()), SLOT(addFiles()));

	addMenu->addAction(_actions["Playlist.AddDir"]);
	connect(_actions["Playlist.AddDir"], SIGNAL(triggered()), SLOT(addDir()));

	connect(_actions["Playlist.AddURL"], SIGNAL(triggered()), SLOT(addURL()));
	addButton->setMenu(addMenu);

	KeyPressEventFilter * deleteKeyFilter = new KeyPressEventFilter(_view, SLOT(clearSelection()),
									Qt::Key_Delete);
	_view->installEventFilter(deleteKeyFilter);

	toolBar->addAction(_actions["Playlist.RemoveAll"]);
	connect(_actions["Playlist.RemoveAll"], SIGNAL(triggered()), _playlistModel, SLOT(clear()));
	connect(_actions["Playlist.RemoveAll"], SIGNAL(triggered()), SLOT(updateWindowTitle()));

	toolBar->addSeparator();

	toolBar->addAction(_actions["Playlist.New"]);
	connect(_actions["Playlist.New"], SIGNAL(triggered()), SLOT(createNewPlaylistWidget()));
}

void PlaylistWidget::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();
	Q_ASSERT(app);

	_actions.add("Playlist.Open", new QAction(app));
	_actions.add("Playlist.Save", new QAction(app));

	_actions.add("Playlist.Add", new QAction(app));
	_actions.add("Playlist.AddFiles", new QAction(app));
	_actions.add("Playlist.AddDir", new QAction(app));
	_actions.add("Playlist.AddURL", new QAction(app));

	_actions.add("Playlist.RemoveAll", new QAction(app));

	TkAction * action = new TkAction(app, tr("Ctrl+S"));
	action->setCheckable(true);
	_actions.add("Playlist.Shuffle", action);

	action = new TkAction(app, tr("Ctrl+R"));
	action->setCheckable(true);
	_actions.add("Playlist.Repeat", action);

	_actions.add("Playlist.JumpToCurrent", new TkAction(app, tr("Ctrl+J")));

	_actions.add("Playlist.New", new QAction(app));
}

void PlaylistWidget::retranslate() {
	_searchLineEdit->setToolTip(tr("Search playlist, use whitespaces to separate words"));
	_searchLineEdit->setClickMessage(tr("Search"));

	_actions["Playlist.Open"]->setText(tr("Open Playlist"));
	_actions["Playlist.Open"]->setIcon(QIcon::fromTheme("document-open"));

	_actions["Playlist.Save"]->setText(tr("Save Playlist"));
	_actions["Playlist.Save"]->setIcon(QIcon::fromTheme("document-save"));

	_actions["Playlist.Add"]->setText(tr("Add..."));
	_actions["Playlist.Add"]->setIcon(QIcon::fromTheme("list-add"));

	_actions["Playlist.AddFiles"]->setText(tr("Add Files"));
	_actions["Playlist.AddDir"]->setText(tr("Add Directory"));
	_actions["Playlist.AddURL"]->setText(tr("Add URL"));

	_actions["Playlist.RemoveAll"]->setText(tr("Remove All"));
	_actions["Playlist.RemoveAll"]->setIcon(QIcon::fromTheme("list-remove"));

	_actions["Playlist.Shuffle"]->setText(tr("Shuffle"));
	_actions["Playlist.Shuffle"]->setIcon(QIcon::fromTheme("media-playlist-shuffle"));

	_actions["Playlist.Repeat"]->setText(tr("Repeat"));
	_actions["Playlist.Repeat"]->setIcon(QIcon::fromTheme("media-playlist-repeat"));

	_actions["Playlist.JumpToCurrent"]->setText(tr("Jump to Current Playing Media"));
	_actions["Playlist.JumpToCurrent"]->setIcon(QIcon::fromTheme("go-jump"));

	_actions["Playlist.New"]->setText(tr("New Playlist Window"));
	_actions["Playlist.New"]->setIcon(QIcon::fromTheme("tab-new"));

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

		addFilesToPlaylist(files);
	}
}

void PlaylistWidget::filesOpenedByMainWindow(const QStringList & files) {
	int position = _playlistModel->files().count();
	addFilesToPlaylist(files);
	_playlistModel->setPosition(position);
	jumpToCurrent();
}

void PlaylistWidget::addFilesToPlaylist(const QStringList & files) {
	if (!files.isEmpty()) {
		if (PlaylistConfig::instance().activePlaylist() == uuid()) {
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
	QString fileName = TkFileDialog::getOpenFileName(
		this, tr("Select Playlist File"), Config::instance().lastDirOpened(),
		tr("Playlist") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Playlist)) + ";;" +
		tr("All Files") + " (*.*)"
	);

	if (!fileName.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIR_OPENED_KEY, QFileInfo(fileName).absolutePath());
		_playlistModel->clear();
		_playlistModel->loadPlaylist(fileName);
	}
}

void PlaylistWidget::playlistLoaded(PlaylistParser::Error, int timeElapsed) {
	PlaylistDebug() << "Load time (seconds):" << timeElapsed / 1000;
	updateWindowTitle();
}

void PlaylistWidget::playlistSaved(PlaylistParser::Error, int timeElapsed) {
	PlaylistDebug() << "Save time (seconds):" << timeElapsed / 1000;
	updateWindowTitle();
}

void PlaylistWidget::updateWindowTitle(const QString & statusMessage) {
	QString nbMedias(QString::number(_playlistModel->rowCount()));
	QString windowTitle(nbMedias + ' ' + tr("medias", "", _playlistModel->rowCount()));
	if (!statusMessage.isEmpty()) {
		windowTitle += " - " + statusMessage;
	}
	_dockWidget->setWindowTitle(windowTitle);
	QStatusBar * statusBar = _mainWindow->statusBar();
	if (statusBar) {
		statusBar->showMessage(statusMessage);
	}
}

void PlaylistWidget::savePlaylist() {
	static const char * PLAYLIST_DEFAULT_EXTENSION = "m3u";

	QString fileName = TkFileDialog::getSaveFileName(
		this, tr("Save Playlist"), Config::instance().lastDirOpened(),
		FileTypes::toSaveFilterFormat(FileTypes::extensions(FileType::Playlist), PLAYLIST_DEFAULT_EXTENSION) +
		tr("All Files") + " (*.*)"
	);

	if (!fileName.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIR_OPENED_KEY, QFileInfo(fileName).absolutePath());

		static PlaylistWriter * parser = new PlaylistWriter(this);
		connect(parser, SIGNAL(finished(PlaylistParser::Error, int)),
			SLOT(playlistSaved(PlaylistParser::Error, int)));
		parser->save(fileName, _playlistModel->files());
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
	connect(Actions["Common.NextTrack"], SIGNAL(triggered()),
		_playlistFilter, SLOT(playNextTrack()));

	//Previous track
	connect(Actions["Common.PreviousTrack"], SIGNAL(triggered()),
		_playlistFilter, SLOT(playPreviousTrack()));
}

void PlaylistWidget::disconnectFromMediaObjectList() {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
		tmp->disconnect(_playlistFilter);
	}

	//Next track
	Actions["Common.NextTrack"]->disconnect(_playlistFilter);

	//Previous track
	Actions["Common.PreviousTrack"]->disconnect(_playlistFilter);
}

void PlaylistWidget::createNewPlaylistWidget() {
	quarkPlayer().pluginManager().loadDisabledPlugin(PlaylistWidgetFactory::PLUGIN_NAME);
}

void PlaylistWidget::jumpToCurrent() {
	QModelIndex index = _playlistFilter->currentIndex();
	if (!_playlistFilter->mapFromSource(index).isValid()) {
		//_searchLineEdit->clear();
		search();
	}
	_view->scrollTo(_playlistFilter->mapFromSource(index), QAbstractItemView::PositionAtCenter);
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
	PlaylistDebug() << tmp;

	_playlistFilter->setFilterRegExp(QRegExp(tmp,
			Qt::CaseInsensitive, QRegExp::RegExp2));

	//Force the treeView to launch files tags fetching
	//FIXME Does not work
	//_view->repaint();
	///

	if (!pattern.isEmpty()) {
		updateWindowTitle(tr("Search finished:") + ' ' + QString::number((float) timeElapsed.elapsed() / 1000) + ' ' + tr("seconds") +
			" (" + QString::number(_playlistFilter->rowCount()) + ' ' + tr("medias", "", _playlistFilter->rowCount()) + ')');
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
	PlaylistDebug() << "Word:" << word;
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
	if (_uuid == uuid()) {
		//This playlist is the active one

		disconnectFromMediaObjectList();
		connectToMediaObject(quarkPlayer().currentMediaObject());
	} else {
		//This playlist is not the active one

		disconnectFromMediaObjectList();
	}
}
