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

#include "MainWindow.h"

#include "QuarkPlayer.h"
#include "AboutWindow.h"
#include "config/Config.h"
#include "config/ConfigWindow.h"
#include "version.h"

#include <tkutil/ActionCollection.h>
#include <tkutil/TkIcon.h>
#include <tkutil/TkFile.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <filetypes/FileTypes.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtCore/QSignalMapper>
#include <QtCore/QCoreApplication>

#include <QtGui/QtGui>

#include <cstdio>

MainWindow::MainWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid, QWidget * parent)
	: TkMainWindow(parent),
	PluginInterface(quarkPlayer, uuid) {

	populateActionCollection();

	setupUi();

	//Accepts Drag&Drop
	setAcceptDrops(true);

	addRecentFilesToMenu();

	_playToolBar = NULL;
	_statusBar = NULL;
	_configWindow = NULL;

	connect(ActionCollection::action("playFile"), SIGNAL(triggered()), SLOT(playFile()));
	connect(ActionCollection::action("playDVD"), SIGNAL(triggered()), SLOT(playDVD()));
	connect(ActionCollection::action("playURL"), SIGNAL(triggered()), SLOT(playURL()));
	connect(ActionCollection::action("newMediaObject"), SIGNAL(triggered()), &quarkPlayer, SLOT(createNewMediaObject()));
	connect(ActionCollection::action("configure"), SIGNAL(triggered()), SLOT(showConfigWindow()));
	connect(ActionCollection::action("quit"), SIGNAL(triggered()), SLOT(close()));
	connect(ActionCollection::action("reportBug"), SIGNAL(triggered()), SLOT(reportBug()));
	connect(ActionCollection::action("about"), SIGNAL(triggered()), SLOT(about()));
	connect(ActionCollection::action("aboutQt"), SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	bool ok = restoreGeometry(Config::instance().mainWindowGeometry());
	if (!ok) {
		qCritical() << __FUNCTION__ << "Error: coudn't restore the main window geometry";
	}

	RETRANSLATE(this);
	retranslate();
}

MainWindow::~MainWindow() {
}

void MainWindow::setPlayToolBar(QToolBar * playToolBar) {
	_playToolBar = playToolBar;
	addToolBar(Qt::BottomToolBarArea, playToolBar);
	qDebug() << __FUNCTION__;
	emit playToolBarAdded(_playToolBar);
}

QToolBar * MainWindow::playToolBar() const {
	return _playToolBar;
}

void MainWindow::setStatusBar(QStatusBar * statusBar) {
	_statusBar = statusBar;
	if (_statusBar) {
		//If statusBar == NULL then QMainWindow::setStatusBar(NULL)
		//will call statusBar destructor
		//This can lead to an infinite loop since the statusbar plugin
		//does a MainWindow::setStatusBar(NULL) inside its destructor
		QMainWindow::setStatusBar(statusBar);
		emit statusBarAdded(_statusBar);
	}
}

QStatusBar * MainWindow::statusBar() const {
	return _statusBar;
}

void MainWindow::addRecentFilesToMenu() {
	connect(ActionCollection::action("clearRecentFiles"), SIGNAL(triggered()), SLOT(clearRecentFiles()));

	QStringList recentFiles = Config::instance().recentFiles();
	if (!recentFiles.isEmpty()) {

		_menuRecentFiles->clear();

		QSignalMapper * signalMapper = new QSignalMapper(this);

		for (int i = 0; i < recentFiles.size(); i++) {
			QString filename = recentFiles[i];
			QUrl url(filename);
			if (url.host().isEmpty()) {
				//Then we have a local file and not a real url (i.e "http://blabla")
				filename = TkFile::fileName(filename);
				filename = filename.right(filename.length() - filename.lastIndexOf('/') - 1);
			}

			QAction * action = _menuRecentFiles->addAction(filename, signalMapper, SLOT(map()));
			signalMapper->setMapping(action, i);
		}

		connect(signalMapper, SIGNAL(mapped(int)),
			SLOT(playRecentFile(int)));
	} else {
		_menuRecentFiles->addAction(ActionCollection::action("emptyMenu"));
	}

	_menuRecentFiles->addSeparator();
	_menuRecentFiles->addAction(ActionCollection::action("clearRecentFiles"));
}

void MainWindow::playRecentFile(int id) {
	QStringList recentFiles = Config::instance().recentFiles();
	play(recentFiles[id]);
}

void MainWindow::clearRecentFiles() {
	//Clear recent files menu
	_menuRecentFiles->clear();
	_menuRecentFiles->addAction(ActionCollection::action("emptyMenu"));
	_menuRecentFiles->addSeparator();
	_menuRecentFiles->addAction(ActionCollection::action("clearRecentFiles"));

	//Clear recent files configuration
	Config::instance().setValue(Config::RECENT_FILES_KEY, QStringList());
}

void MainWindow::playFile() {
	QString filename = TkFileDialog::getOpenFileName(
		this, tr("Select Audio/Video File"), Config::instance().lastDirectoryUsed(),
		tr("Multimedia") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video, FileType::Audio)) + ";;" +
		tr("Video") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video)) +";;" +
		tr("Audio") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Audio)) +";;" +
		tr("Playlist") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Playlist)) + ";;" +
		tr("All Files") + " (*)"
	);

	if (!filename.isEmpty()) {
		Config::instance().setValue(Config::LAST_DIRECTORY_USED_KEY, QFileInfo(filename).absolutePath());

		play(filename);
	}
}

void MainWindow::playDVD() {
	QString dir = TkFileDialog::getExistingDirectory(this, tr("Select DVD folder"),
			Config::instance().lastDirectoryUsed());

	if (!dir.isEmpty()) {
		play(Phonon::MediaSource(Phonon::Dvd, dir));
	}
}

void MainWindow::playURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));

	if (!url.isEmpty()) {
		play(url);
	}
}

void MainWindow::play(const Phonon::MediaSource & mediaSource) {
	addFileToRecentFilesMenu(mediaSource);
	quarkPlayer().play(mediaSource);
	//_statusBar->showMessage(tr("Processing") + " " + filename + "...");
}

void MainWindow::addFileToRecentFilesMenu(const Phonon::MediaSource & mediaSource) {
	static const int MAX_RECENT_FILES = 10;

	QString filename = mediaSource.fileName();
	if (filename.isEmpty()) {
		//filename is not a local file, so maybe an url
		//it can be a DVD too, but then we don't add it the list of recent files
		filename = mediaSource.url().toString();
	}

	//Add the file to the list of recent files opened
	if (!filename.isEmpty()) {
		//filename is either a local file or an url
		//We don't add other types of media inside the list of recent files opened
		QStringList recentFiles = Config::instance().recentFiles();
		if (recentFiles.contains(filename)) {
			recentFiles.removeAll(filename);
		}
		recentFiles.prepend(filename);
		if (recentFiles.size() > MAX_RECENT_FILES) {
			recentFiles.removeLast();
		}

		Config::instance().setValue(Config::RECENT_FILES_KEY, recentFiles);

		addRecentFilesToMenu();
	}
}

void MainWindow::updateWindowTitle() {
	QString title = quarkPlayer().currentMediaObjectTitle();

	if (title.isEmpty()) {
		setWindowTitle(QCoreApplication::applicationName() + "-" + QString(QUARKPLAYER_VERSION));
	} else {
		setWindowTitle(title + " - " + QCoreApplication::applicationName());
	}
}

void MainWindow::showConfigWindow() {
	if (!_configWindow) {
		_configWindow = new ConfigWindow(this);

		//Emits the signal just once, not each time the ConfigWindow is being showed
		emit configWindowCreated(_configWindow);
	}

	_configWindow->show();
}

ConfigWindow * MainWindow::configWindow() const {
	return _configWindow;
}

void MainWindow::reportBug() {
	QDesktopServices::openUrl(QUrl("http://code.google.com/p/phonon-vlc-mplayer/issues/list"));
}

void MainWindow::about() {
	static AboutWindow * aboutWindow = new AboutWindow(this);
	aboutWindow->show();
}

void MainWindow::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("playFile", new QAction(app));
	ActionCollection::addAction("quit", new QAction(app));
	ActionCollection::addAction("reportBug", new QAction(app));
	ActionCollection::addAction("about", new QAction(app));
	ActionCollection::addAction("aboutQt", new QAction(app));
	ActionCollection::addAction("playDVD", new QAction(app));
	ActionCollection::addAction("playURL", new QAction(app));
	ActionCollection::addAction("playVCD", new QAction(app));
	ActionCollection::addAction("newMediaObject", new QAction(app));
	ActionCollection::addAction("equalizer", new QAction(app));
	ActionCollection::addAction("configure", new QAction(app));
	ActionCollection::addAction("clearRecentFiles", new QAction(app));
	ActionCollection::addAction("emptyMenu", new QAction(app));

	ActionCollection::addAction("previousTrack", new QAction(app));
	ActionCollection::addAction("play", new QAction(app));
	ActionCollection::addAction("pause", new QAction(app));
	ActionCollection::addAction("stop", new QAction(app));
	ActionCollection::addAction("nextTrack", new QAction(app));

	//FIXME See MainWindow.cpp MediaController.cpp FindSubtitles.cpp QuarkPlayer.h
	//Need to implement a full plugin system like Qt Creator has
	//Let's wait for Qt Creator source code to be released...
	//This way MainWindow would be also a real plugin!
	ActionCollection::addAction("findSubtitles", new QAction(app));
	ActionCollection::addAction("uploadSubtitles", new QAction(app));
	///

	QAction * action = new QAction(app);
	action->setCheckable(true);
	ActionCollection::addAction("fullScreen", action);
}

void MainWindow::setupUi() {
	//No central widget, only QDockWidget
	setCentralWidget(NULL);

	_menuFile = new QMenu();
	menuBar()->addAction(_menuFile->menuAction());
	_menuFile->addAction(ActionCollection::action("playFile"));
	_menuRecentFiles = new QMenu();
	_menuFile->addAction(_menuRecentFiles->menuAction());
	_menuFile->addAction(ActionCollection::action("playDVD"));
	_menuFile->addAction(ActionCollection::action("playURL"));
	_menuFile->addAction(ActionCollection::action("playVCD"));
	_menuFile->addSeparator();
	_menuFile->addAction(ActionCollection::action("quit"));

	_menuPlay = new QMenu();
	menuBar()->addAction(_menuPlay->menuAction());
	_menuPlay->addAction(ActionCollection::action("previousTrack"));
	_menuPlay->addAction(ActionCollection::action("play"));
	_menuPlay->addAction(ActionCollection::action("pause"));
	_menuPlay->addAction(ActionCollection::action("stop"));
	_menuPlay->addAction(ActionCollection::action("nextTrack"));
	_menuPlay->addSeparator();
	_menuPlay->addAction(ActionCollection::action("fullScreen"));
	_menuPlay->addSeparator();
	_menuPlay->addAction(ActionCollection::action("newMediaObject"));

	_menuSettings = new QMenu();
	menuBar()->addAction(_menuSettings->menuAction());
	_menuSettings->addAction(ActionCollection::action("equalizer"));
	_menuSettings->addAction(ActionCollection::action("configure"));

	_menuHelp = new QMenu();
	menuBar()->addAction(_menuHelp->menuAction());
	_menuHelp->addAction(ActionCollection::action("reportBug"));
	_menuHelp->addAction(ActionCollection::action("about"));
	_menuHelp->addAction(ActionCollection::action("aboutQt"));

	//Main ToolBar
	_mainToolBar = new TkToolBar(this);
	//_mainToolBar->setIconSize(QSize(16, 16));

	_mainToolBar->addAction(ActionCollection::action("playFile"));
	_mainToolBar->addAction(ActionCollection::action("playDVD"));
	_mainToolBar->addAction(ActionCollection::action("playURL"));
	_mainToolBar->addSeparator();
	_mainToolBar->addAction(ActionCollection::action("equalizer"));
	_mainToolBar->addAction(ActionCollection::action("configure"));
	addToolBar(_mainToolBar);
}

void MainWindow::retranslate() {
	updateWindowTitle();
	setWindowIcon(QIcon(":/icons/hi16-app-quarkplayer.png"));

	ActionCollection::action("playFile")->setText(tr("Play &File..."));
	ActionCollection::action("playFile")->setIcon(TkIcon("document-open"));

	ActionCollection::action("quit")->setText(tr("&Quit"));
	ActionCollection::action("quit")->setIcon(TkIcon("application-exit"));

	ActionCollection::action("reportBug")->setText(tr("&Report a bug..."));
	ActionCollection::action("reportBug")->setIcon(TkIcon("tools-report-bug"));

	ActionCollection::action("about")->setText(tr("&About"));
	ActionCollection::action("about")->setIcon(TkIcon("help-about"));

	ActionCollection::action("aboutQt")->setText(tr("About &Qt"));
	ActionCollection::action("aboutQt")->setIcon(TkIcon("help-about"));

	ActionCollection::action("playDVD")->setText(tr("Play &DVD"));
	ActionCollection::action("playDVD")->setIcon(TkIcon("media-optical"));

	ActionCollection::action("playURL")->setText(tr("Play &URL..."));
	ActionCollection::action("playURL")->setIcon(TkIcon("document-open-remote"));

	ActionCollection::action("playVCD")->setText(tr("Play &VCD"));
	//ActionCollection::action("playVCD")->setIcon(TkIcon("media-optical"));

	ActionCollection::action("newMediaObject")->setText(tr("New Media Window"));
	ActionCollection::action("newMediaObject")->setIcon(TkIcon("window-new"));

	ActionCollection::action("equalizer")->setText(tr("&Equalizer..."));
	ActionCollection::action("equalizer")->setIcon(TkIcon("view-media-equalizer"));

	ActionCollection::action("configure")->setText(tr("&Configure QuarkPlayer..."));
	ActionCollection::action("configure")->setIcon(TkIcon("preferences-system"));

	ActionCollection::action("clearRecentFiles")->setText(tr("&Clear"));
	ActionCollection::action("clearRecentFiles")->setIcon(TkIcon("edit-delete"));

	ActionCollection::action("emptyMenu")->setText(tr("<empty>"));
	ActionCollection::action("emptyMenu")->setEnabled(false);

	_mainToolBar->setWindowTitle(tr("Main ToolBar"));
	_mainToolBar->setMinimumSize(_mainToolBar->sizeHint());

	_menuRecentFiles->setTitle(tr("&Recent Files"));
	_menuRecentFiles->setIcon(TkIcon("document-open-recent"));

	_menuFile->setTitle(tr("&File"));
	_menuPlay->setTitle(tr("&Play"));
	_menuSettings->setTitle(tr("&Settings"));
	_menuHelp->setTitle(tr("&Help"));

	ActionCollection::action("previousTrack")->setText(tr("P&revious Track"));
	ActionCollection::action("previousTrack")->setIcon(TkIcon("media-skip-backward"));

	ActionCollection::action("play")->setText(tr("P&lay"));
	ActionCollection::action("play")->setIcon(TkIcon("media-playback-start"));

	ActionCollection::action("pause")->setText(tr("&Pause"));
	ActionCollection::action("pause")->setIcon(TkIcon("media-playback-pause"));

	ActionCollection::action("stop")->setText(tr("&Stop"));
	ActionCollection::action("stop")->setIcon(TkIcon("media-playback-stop"));

	ActionCollection::action("nextTrack")->setText(tr("&Next Track"));
	ActionCollection::action("nextTrack")->setIcon(TkIcon("media-skip-forward"));

	ActionCollection::action("fullScreen")->setText(tr("&FullScreen"));
	ActionCollection::action("fullScreen")->setIcon(TkIcon("view-fullscreen"));
}

QMenu * MainWindow::menuFile() const {
	return _menuFile;
}

QMenu * MainWindow::menuPlay() const {
	return _menuPlay;
}

QMenu * MainWindow::menuSettings() const {
	return _menuSettings;
}

QMenu * MainWindow::menuHelp() const {
	return _menuHelp;
}

void MainWindow::dragEnterEvent(QDragEnterEvent * event) {
	qDebug() << __FUNCTION__ << event->mimeData()->formats();

	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent * event) {
	QStringList files;

	//Add urls to a list
	if (event->mimeData()->hasUrls()) {
		QList<QUrl> urlList = event->mimeData()->urls();
		QString filename;
		foreach (QUrl url, urlList) {
			if (url.isValid()) {
				qDebug() << __FUNCTION__ << "File scheme:" << url.scheme();
				if (url.scheme() == "file") {
					filename = url.toLocalFile();
				} else {
					filename = url.toString();
				}
				qDebug() << __FUNCTION__ << "File dropped:" << filename;
				files << filename;
			}
		}
	}

	if (files.count() > 0) {
		if (files.count() == 1) {
			//1 file
			QString filename = files[0];

			bool isSubtitle = FileTypes::extensions(FileType::Subtitle).contains(TkFile::fileExtension(filename), Qt::CaseInsensitive);
			if (isSubtitle) {
				qDebug() << __FUNCTION__ << "Loading subtitle:" << filename;
				emit subtitleFileDropped(filename);
			} else if (TkFile::isDir(filename)) {
				//TODO open directory
			} else {
				//TODO add to playlist if 'auto-add-playlist' option
				play(filename);
			}
		} else {
			//Several files

			//TODO add files to playlist
			//and play the first one in the list
		}
	}
}

void MainWindow::closeEvent(QCloseEvent * event) {
	//Saves window geometry
	Config::instance().setValue(Config::MAINWINDOW_GEOMETRY_KEY, saveGeometry());

	TkMainWindow::closeEvent(event);

	//Quits the application
	QCoreApplication::quit();

	//FIXME we should only use QCoreApplication::quit()
	//exit(EXIT_SUCCESS);
}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockWidget, QDockWidget * lastDockWidget) {
	if (dockWidget) {
		QMainWindow::addDockWidget(area, dockWidget);
		if (lastDockWidget) {
			tabifyDockWidget(lastDockWidget, dockWidget);
		}
	}
}

void MainWindow::addBrowserDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::LeftDockWidgetArea, dockWidget, lastDockWidget);
	lastDockWidget = dockWidget;
}

void MainWindow::resetBrowserDockWidget() {
	addBrowserDockWidget(NULL);
}

void MainWindow::addVideoDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::RightDockWidgetArea, dockWidget, lastDockWidget);
	lastDockWidget = dockWidget;
}

void MainWindow::resetVideoDockWidget() {
	addVideoDockWidget(NULL);
}

void MainWindow::addPlaylistDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::RightDockWidgetArea, dockWidget, lastDockWidget);
	lastDockWidget = dockWidget;
}

void MainWindow::resetPlaylistDockWidget() {
	addPlaylistDockWidget(NULL);
}

void MainWindow::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	//Resets the main window title
	updateWindowTitle();
	connect(mediaObject, SIGNAL(metaDataChanged()), SLOT(updateWindowTitle()));

	disconnect(ActionCollection::action("quit"), SIGNAL(triggered()), mediaObject, SLOT(stop()));
	connect(ActionCollection::action("quit"), SIGNAL(triggered()), mediaObject, SLOT(stop()));
}
