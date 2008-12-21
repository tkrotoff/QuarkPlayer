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
#include "UuidActionCollection.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/config/PlaylistConfig.h>
#include <quarkplayer/PluginsManager.h>

#include <tkutil/TkIcon.h>
#include <tkutil/ActionCollection.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>
#include <tkutil/KeyEventFilter.h>
#include <tkutil/SearchLineEdit.h>

#include <filetypes/FileTypes.h>

#include <playlistparser/PlaylistParser.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

static const char * PLAYLIST_SEARCH_HISTORY_KEY = "playlist_search_history";

Q_EXPORT_PLUGIN2(PlaylistWidget, PlaylistWidgetFactory);

PluginInterface * PlaylistWidgetFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new PlaylistWidget(quarkPlayer, uuid);
}

PlaylistWidget::PlaylistWidget(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QWidget(quarkPlayer.mainWindow()),
	PluginInterface(quarkPlayer, uuid) {

	//Short for UuidActionCollection::setUuid()
	setUuid(uuid);
	///

	//Model
	_playlistModel = new PlaylistModel(this, quarkPlayer, uuid);
	connect(_playlistModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
		SLOT(updateWindowTitle()));
	connect(_playlistModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
		SLOT(updateWindowTitle()));
	connect(_playlistModel, SIGNAL(playlistLoaded(int)),
		SLOT(playlistLoaded(int)));
	connect(_playlistModel, SIGNAL(playlistSaved(int)),
		SLOT(playlistSaved(int)));

	//Filter
	_playlistFilter = new PlaylistFilter(this, _playlistModel);

	//TreeView
	_treeView = new DragAndDropTreeView(_playlistModel, _playlistFilter, uuid);
	connect(_treeView, SIGNAL(activated(const QModelIndex &)),
		_playlistFilter, SLOT(play(const QModelIndex &)));
	_treeView->setModel(_playlistFilter);
	QVBoxLayout * layout = new QVBoxLayout();
	setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);

	//Default column sizes
	_treeView->resizeColumnToContents(PlaylistModel::COLUMN_TRACK);
	_treeView->setColumnWidth(PlaylistModel::COLUMN_TITLE, 200);
	_treeView->setColumnWidth(PlaylistModel::COLUMN_ARTIST, 150);
	_treeView->setColumnWidth(PlaylistModel::COLUMN_ALBUM, 150);
	_treeView->resizeColumnToContents(PlaylistModel::COLUMN_LENGTH);

	populateActionCollection();
	createToolBar();

	layout->addWidget(_treeView);

	//Add to the main window
	_dockWidget = new QDockWidget();
	connect(_dockWidget, SIGNAL(visibilityChanged(bool)),
		SLOT(dockWidgetVisibilityChanged(bool)));
	quarkPlayer.mainWindow()->addPlaylistDockWidget(_dockWidget);
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
	quarkPlayer().mainWindow()->removeDockWidget(_dockWidget);
	quarkPlayer().mainWindow()->resetPlaylistDockWidget();
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

	toolBar->addAction(uuidAction("playlistShuffle"));
	connect(uuidAction("playlistShuffle"), SIGNAL(toggled(bool)), _playlistFilter, SLOT(setShuffle(bool)));
	toolBar->addAction(uuidAction("playlistRepeat"));
	connect(uuidAction("playlistRepeat"), SIGNAL(toggled(bool)), _playlistFilter, SLOT(setRepeat(bool)));

	toolBar->addAction(uuidAction("playlistJumpToCurrent"));
	connect(uuidAction("playlistJumpToCurrent"), SIGNAL(triggered()), SLOT(jumpToCurrent()));

	//Search line edit
	QStringList history = Config::instance().value(PLAYLIST_SEARCH_HISTORY_KEY).toStringList();
	_searchLineEdit = new SearchLineEdit(history, toolBar);
	connect(_searchLineEdit, SIGNAL(textChanged(const QString &)), SLOT(search()));
	toolBar->addWidget(_searchLineEdit);

	toolBar->addAction(uuidAction("playlistOpen"));
	connect(uuidAction("playlistOpen"), SIGNAL(triggered()), SLOT(openPlaylist()));
	toolBar->addAction(uuidAction("playlistSave"));
	connect(uuidAction("playlistSave"), SIGNAL(triggered()), SLOT(savePlaylist()));

	//We have to use a QToolButton instead of a QAction,
	//otherwise we cannot use QToolButton::InstantPopup :/
	QToolButton * addButton = new QToolButton();
	addButton->setPopupMode(QToolButton::InstantPopup);
	addButton->setDefaultAction(uuidAction("playlistAdd"));
	toolBar->addWidget(addButton);

	QMenu * addMenu = new QMenu();
	addMenu->addAction(uuidAction("playlistAddFiles"));
	connect(uuidAction("playlistAddFiles"), SIGNAL(triggered()), SLOT(addFiles()));
	addMenu->addAction(uuidAction("playlistAddDirectory"));
	connect(uuidAction("playlistAddDirectory"), SIGNAL(triggered()), SLOT(addDir()));
	addMenu->addAction(uuidAction("playlistAddURL"));
	connect(uuidAction("playlistAddURL"), SIGNAL(triggered()), SLOT(addURL()));
	addButton->setMenu(addMenu);

	KeyPressEventFilter * deleteKeyFilter = new KeyPressEventFilter(_treeView, SLOT(clearSelection()), Qt::Key_Delete);
	_treeView->installEventFilter(deleteKeyFilter);

	toolBar->addAction(uuidAction("playlistRemoveAll"));
	connect(uuidAction("playlistRemoveAll"), SIGNAL(triggered()), _playlistModel, SLOT(clear()));
	connect(uuidAction("playlistRemoveAll"), SIGNAL(triggered()), SLOT(updateWindowTitle()));

	toolBar->addSeparator();

	toolBar->addAction(uuidAction("playlistNew"));
	connect(uuidAction("playlistNew"), SIGNAL(triggered()), SLOT(createNewPlaylistWidget()));
}

void PlaylistWidget::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	addUuidAction("playlistOpen", new QAction(app));
	addUuidAction("playlistSave", new QAction(app));

	addUuidAction("playlistAdd", new QAction(app));
	addUuidAction("playlistAddFiles", new QAction(app));
	addUuidAction("playlistAddDirectory", new QAction(app));
	addUuidAction("playlistAddURL", new QAction(app));

	addUuidAction("playlistRemoveAll", new QAction(app));

	QAction * action = new QAction(app);
	action->setCheckable(true);
	addUuidAction("playlistShuffle", action);
	action = new QAction(app);
	action->setCheckable(true);
	addUuidAction("playlistRepeat", action);

	addUuidAction("playlistJumpToCurrent", new QAction(app));

	addUuidAction("playlistClearSearch", new QAction(app));

	addUuidAction("playlistNew", new QAction(app));
}

void PlaylistWidget::retranslate() {
	_searchLineEdit->clearButton()->setToolTip(tr("Clear Search"));
	_searchLineEdit->clearButton()->setIcon(TkIcon("edit-clear-locationbar-rtl"));

	_searchLineEdit->showWordListButton()->setToolTip(tr("Previous Searches"));
	_searchLineEdit->showWordListButton()->setIcon(TkIcon("go-down-search"));

	_searchLineEdit->setToolTip(tr("Search the playlist, use whitespaces to separate words"));
	_searchLineEdit->setClickMessage(tr("Enter search terms here"));

	uuidAction("playlistOpen")->setText(tr("Open Playlist"));
	uuidAction("playlistOpen")->setIcon(TkIcon("document-open"));

	uuidAction("playlistSave")->setText(tr("Save Playlist"));
	uuidAction("playlistSave")->setIcon(TkIcon("document-save"));

	uuidAction("playlistAdd")->setText(tr("Add..."));
	uuidAction("playlistAdd")->setIcon(TkIcon("list-add"));

	uuidAction("playlistAddFiles")->setText(tr("Add Files"));
	uuidAction("playlistAddDirectory")->setText(tr("Add Directory"));
	uuidAction("playlistAddURL")->setText(tr("Add URL"));

	uuidAction("playlistRemoveAll")->setText(tr("Remove All"));
	uuidAction("playlistRemoveAll")->setIcon(TkIcon("list-remove"));

	uuidAction("playlistShuffle")->setText(tr("Shuffle"));
	uuidAction("playlistShuffle")->setIcon(TkIcon("media-playlist-shuffle"));

	uuidAction("playlistRepeat")->setText(tr("Repeat"));
	uuidAction("playlistRepeat")->setIcon(TkIcon("media-playlist-repeat"));

	uuidAction("playlistJumpToCurrent")->setText(tr("Jump to Current Playing Media"));
	uuidAction("playlistJumpToCurrent")->setIcon(TkIcon("go-jump"));

	uuidAction("playlistNew")->setText(tr("New Playlist Window"));
	uuidAction("playlistNew")->setIcon(TkIcon("window-new"));

	updateWindowTitle();
}

void PlaylistWidget::addFiles() {
	QStringList files = TkFileDialog::getOpenFileNames(
		this, tr("Select Audio/Video Files"), Config::instance().lastDirUsed(),
		tr("Multimedia") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video, FileType::Audio)) + ";;" +
		tr("Video") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video)) +";;" +
		tr("Audio") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Audio)) +";;" +
		tr("Playlist") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Playlist)) + ";;" +
		tr("All Files") + " (*.*)"
	);

	if (!files.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIR_USED_KEY, QFileInfo(files[0]).absolutePath());

		_playlistModel->addFiles(files);
		_playlistModel->saveCurrentPlaylist();
	}
}

void PlaylistWidget::addDir() {
	QStringList files;

	QString dir = TkFileDialog::getExistingDirectory(this, tr("Select Directory"),
			Config::instance().lastDirUsed());
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
		this, tr("Select Playlist File"), Config::instance().lastDirUsed(),
		tr("Playlist") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Playlist)) + ";;" +
		tr("All Files") + " (*.*)"
	);

	if (!filename.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIR_USED_KEY, QFileInfo(filename).absolutePath());

		PlaylistParser * parser = new PlaylistParser(filename, this);
		connect(parser, SIGNAL(filesFound(const QStringList &)),
			SLOT(parserFilesFound(const QStringList &)));
		connect(parser, SIGNAL(finished(int)),
			SLOT(playlistLoaded(int)));
		connect(parser, SIGNAL(finished(int)),
			_playlistModel, SLOT(saveCurrentPlaylist()));
		parser->load();
	}
}

void PlaylistWidget::parserFilesFound(const QStringList & files) {
	_playlistModel->addFiles(files);
}

void PlaylistWidget::playlistLoaded(int timeElapsed) {
	updateWindowTitle(tr("Playlist loaded:") + ' ' + QString::number((float) timeElapsed / 1000) + ' ' + tr("seconds") +
			" (" + QString::number(_playlistModel->rowCount()) + " " + tr("medias") + ")");
}

void PlaylistWidget::playlistSaved(int timeElapsed) {
	updateWindowTitle(tr("Playlist saved:") + ' ' + QString::number((float) timeElapsed / 1000) + ' ' + tr("seconds") +
			" (" + QString::number(_playlistModel->rowCount()) + " " + tr("medias") + ")");
}

void PlaylistWidget::updateWindowTitle(const QString & statusMessage) {
	QString nbMedias(QString::number(_playlistModel->rowCount()));
	QString windowTitle(nbMedias + " " + tr("medias"));
	if (!statusMessage.isEmpty()) {
		windowTitle += " - " + statusMessage;
	}
	_dockWidget->setWindowTitle(windowTitle);
	QStatusBar * statusBar = quarkPlayer().mainWindow()->statusBar();
	if (statusBar) {
		statusBar->showMessage(statusMessage);
	}
}

void PlaylistWidget::savePlaylist() {
	static const char * PLAYLIST_DEFAULT_EXTENSION = "m3u";

	QString filename = TkFileDialog::getSaveFileName(
		this, tr("Save Playlist File"), Config::instance().lastDirUsed(),
		FileTypes::toSaveFilterFormat(FileTypes::extensions(FileType::Playlist), PLAYLIST_DEFAULT_EXTENSION) +
		tr("All Files") + " (*.*)"
	);

	if (!filename.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIR_USED_KEY, QFileInfo(filename).absolutePath());

		PlaylistParser * parser = new PlaylistParser(filename, this);
		connect(parser, SIGNAL(finished(int)),
			SLOT(playlistSaved(int)));
		parser->save(_playlistModel->fileNames());
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
	connect(ActionCollection::action("nextTrack"), SIGNAL(triggered()),
		_playlistFilter, SLOT(playNextTrack()));

	//Previous track
	connect(ActionCollection::action("previousTrack"), SIGNAL(triggered()),
		_playlistFilter, SLOT(playPreviousTrack()));

	//FIXME aboutToFinish does not work properly with DS9 backend???
	//aboutToFinish -> let's queue/play the next track
	connect(mediaObject, SIGNAL(aboutToFinish()),
		_playlistFilter, SLOT(enqueueNextTrack()));

	connect(mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
		SLOT(currentSourceChanged(const Phonon::MediaSource &)));

	connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State)));
}

void PlaylistWidget::disconnectFromMediaObjectList() {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
		tmp->disconnect(_playlistFilter);
	}

	//Next track
	ActionCollection::action("nextTrack")->disconnect(_playlistFilter);

	//Previous track
	ActionCollection::action("previousTrack")->disconnect(_playlistFilter);
}

void PlaylistWidget::currentSourceChanged(const Phonon::MediaSource & source) {
	if (PlaylistConfig::instance().activePlaylist() == uuid()) {
		//Each time the track changes, we enqueue the next track
		//currentSourceChanged() is the only signal that we get when we queue tracks
		_playlistFilter->setPositionAsNextTrack();
	}
}

void PlaylistWidget::stateChanged(Phonon::State newState) {
	switch (newState) {
	case Phonon::ErrorState: {
		Phonon::ErrorType errorType = quarkPlayer().currentMediaObject()->errorType();
		switch (errorType) {
		case Phonon::NoError:
		case Phonon::NormalError:
			//Jump to the next track if possible since the current one is not playable
			_playlistFilter->playNextTrack();
			//TODO add parameter to not repeat again and again the whole playlist
			break;
		case Phonon::FatalError:
			//Do not jump to the next track
			break;
		default:
			qCritical() << "Error: unknown errorType:" << errorType;
		}
		break;
	}

	case Phonon::PlayingState:
		break;

	case Phonon::StoppedState:
		break;

	case Phonon::PausedState:
		break;

	case Phonon::LoadingState:
		break;

	case Phonon::BufferingState:
		break;

	default:
		qCritical() << "Error: unknown newState:" << newState;
	}
}

void PlaylistWidget::createNewPlaylistWidget() {
	PluginData pluginData = PluginsManager::instance().pluginData(uuid());
	PluginsManager::instance().loadDisabledPlugin(pluginData);
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

	if (!pattern.isEmpty()) {
		updateWindowTitle(tr("Search finished:") + ' ' + QString::number((float) timeElapsed.elapsed() / 1000) + ' ' + tr("seconds") +
			" (" + QString::number(_playlistFilter->rowCount()) + " " + tr("medias") + ")");
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
