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
#include "FileExtensions.h"
#include "config/Config.h"
#include "config/ConfigWindow.h"
#include "version.h"

#include <tkutil/ActionCollection.h>
#include <tkutil/TkIcon.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include <QtCore/QSignalMapper>
#include <QtCore/QCoreApplication>

#include <QtGui/QtGui>

MainWindow::MainWindow(QuarkPlayer & quarkPlayer, QWidget * parent)
	: TkMainWindow(parent),
	PluginInterface(quarkPlayer) {

	populateActionCollection();

	setupUi();

	//Accepts Drag&Drop
	setAcceptDrops(true);

	RETRANSLATE(this);
	retranslate();

	addRecentFilesToMenu();

	//playToolBar
	_playToolBar = NULL;
	_statusBar = NULL;
	_videoDockWidget = NULL;
	_browserDockWidget = NULL;
	_playlistDockWidget = NULL;

	connect(ActionCollection::action("playDVD"), SIGNAL(triggered()), SLOT(playDVD()));
	connect(ActionCollection::action("playURL"), SIGNAL(triggered()), SLOT(playURL()));
	connect(ActionCollection::action("playFile"), SIGNAL(triggered()), SLOT(playFile()));
	connect(ActionCollection::action("configure"), SIGNAL(triggered()), SLOT(showConfigWindow()));
	connect(ActionCollection::action("quit"), SIGNAL(triggered()), SLOT(close()));
	connect(ActionCollection::action("about"), SIGNAL(triggered()), SLOT(about()));
	connect(ActionCollection::action("aboutQt"), SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));
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
	QMainWindow::setStatusBar(statusBar);
	emit statusBarAdded(_statusBar);
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
		tr("Multimedia") + FileExtensions::toFilterFormat(FileExtensions::multimedia()) + ";;" +
		tr("Video") + FileExtensions::toFilterFormat(FileExtensions::video()) +";;" +
		tr("Audio") + FileExtensions::toFilterFormat(FileExtensions::audio()) +";;" +
		tr("Playlist") + FileExtensions::toFilterFormat(FileExtensions::playlist()) + ";;" +
		tr("All Files") + " (*)"
	);

	if (QFile::exists(filename)) {
		Config::instance().setValue(Config::LAST_DIRECTORY_USED_KEY, QFileInfo(filename).absolutePath());
	}

	if (!filename.isEmpty()) {
		play(filename);
	}
}

void MainWindow::playDVD() {
	QString directory = TkFileDialog::getExistingDirectory(this, tr("Select DVD folder"),
			Config::instance().lastDirectoryUsed());

	if (!directory.isEmpty()) {
		play(Phonon::MediaSource(Phonon::Dvd, directory));
	}
}

void MainWindow::playURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));
	qDebug() << __FUNCTION__ << "url:" << url;

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

void MainWindow::metaDataChanged() {
	QString title = quarkPlayer().currentMediaObjectTitle();

	if (title.isEmpty()) {
		setWindowTitle(QCoreApplication::applicationName());
	} else {
		setWindowTitle(title + " - " + QCoreApplication::applicationName() + " ");
	}
}

void MainWindow::showConfigWindow() {
	static ConfigWindow * configWindow = new ConfigWindow(this);
	configWindow->show();
}

void MainWindow::about() {
	static AboutWindow * aboutWindow = new AboutWindow(this);
	aboutWindow->show();
}

void MainWindow::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("playFile", new QAction(app));
	ActionCollection::addAction("quit", new QAction(app));
	ActionCollection::addAction("about", new QAction(app));
	ActionCollection::addAction("aboutQt", new QAction(app));
	ActionCollection::addAction("playDVD", new QAction(app));
	ActionCollection::addAction("playURL", new QAction(app));
	ActionCollection::addAction("playVCD", new QAction(app));
	ActionCollection::addAction("equalizer", new QAction(app));
	ActionCollection::addAction("configure", new QAction(app));
	ActionCollection::addAction("openSubtitleFile", new QAction(app));
	ActionCollection::addAction("clearRecentFiles", new QAction(app));
	ActionCollection::addAction("emptyMenu", new QAction(app));
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

	_menuAudio = new QMenu();
	menuBar()->addAction(_menuAudio->menuAction());
	_menuAudioChannels = new QMenu();
	_menuAudioChannels->addAction(ActionCollection::action("emptyMenu"));
	_menuAudio->addAction(_menuAudioChannels->menuAction());

	_menuSubtitle = new QMenu();
	menuBar()->addAction(_menuSubtitle->menuAction());
	_menuSubtitle->addAction(ActionCollection::action("openSubtitleFile"));
	_menuSubtitles = new QMenu();
	_menuSubtitles->addAction(ActionCollection::action("emptyMenu"));
	_menuSubtitle->addAction(_menuSubtitles->menuAction());

	_menuBrowse = new QMenu();
	menuBar()->addAction(_menuBrowse->menuAction());
	_menuTitles = new QMenu();
	_menuTitles->addAction(ActionCollection::action("emptyMenu"));
	_menuBrowse->addAction(_menuTitles->menuAction());
	_menuChapters = new QMenu();
	_menuChapters->addAction(ActionCollection::action("emptyMenu"));
	_menuBrowse->addAction(_menuChapters->menuAction());
	_menuAngles = new QMenu();
	_menuAngles->addAction(ActionCollection::action("emptyMenu"));
	_menuBrowse->addAction(_menuAngles->menuAction());

	_menuSettings = new QMenu();
	menuBar()->addAction(_menuSettings->menuAction());
	_menuSettings->addAction(ActionCollection::action("equalizer"));
	_menuSettings->addAction(ActionCollection::action("configure"));

	_menuHelp = new QMenu();
	menuBar()->addAction(_menuHelp->menuAction());
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
	_mainToolBar->addSeparator();
	_mainToolBar->addAction(ActionCollection::action("quit"));
	addToolBar(_mainToolBar);
}

void MainWindow::retranslate() {
	setWindowTitle(QCoreApplication::applicationName() + "-" + QString(QUARKPLAYER_VERSION));
	setWindowIcon(QIcon(":/icons/hi16-app-quarkplayer.png"));

	ActionCollection::action("playFile")->setText(tr("Play &File..."));
	ActionCollection::action("playFile")->setIcon(TkIcon("document-open"));

	ActionCollection::action("quit")->setText(tr("&Quit"));
	ActionCollection::action("quit")->setIcon(TkIcon("application-exit"));

	ActionCollection::action("about")->setText(tr("&About"));
	ActionCollection::action("about")->setIcon(TkIcon("help-about"));

	ActionCollection::action("aboutQt")->setText(tr("About &Qt"));
	ActionCollection::action("aboutQt")->setIcon(TkIcon("help-about"));

	ActionCollection::action("playDVD")->setText(tr("Play &DVD"));
	ActionCollection::action("playDVD")->setIcon(TkIcon("media-optical"));

	ActionCollection::action("playURL")->setText(tr("Play &URL..."));
	ActionCollection::action("playURL")->setIcon(TkIcon("document-open-remote"));

	ActionCollection::action("playVCD")->setText(tr("Play &VCD"));
	ActionCollection::action("playVCD")->setIcon(TkIcon("media-optical"));

	ActionCollection::action("equalizer")->setText(tr("&Equalizer..."));
	ActionCollection::action("equalizer")->setIcon(TkIcon("view-media-equalizer"));

	ActionCollection::action("configure")->setText(tr("&Configure QuarkPlayer..."));
	ActionCollection::action("configure")->setIcon(TkIcon("preferences-system"));

	ActionCollection::action("openSubtitleFile")->setText(tr("&Open Subtitle..."));
	ActionCollection::action("openSubtitleFile")->setIcon(TkIcon("document-open"));

	ActionCollection::action("clearRecentFiles")->setText(tr("&Clear"));
	ActionCollection::action("clearRecentFiles")->setIcon(TkIcon("edit-delete"));

	ActionCollection::action("emptyMenu")->setText(tr("<empty>"));
	ActionCollection::action("emptyMenu")->setEnabled(false);

	_mainToolBar->setWindowTitle(tr("Main ToolBar"));
	_mainToolBar->setMinimumSize(_mainToolBar->sizeHint());

	_menuRecentFiles->setTitle(tr("&Recent Files"));
	_menuRecentFiles->setIcon(TkIcon("document-open-recent"));

	_menuAudioChannels->setTitle(tr("&Audio Channels"));
	_menuAudioChannels->setIcon(TkIcon("audio-x-generic"));

	_menuSubtitles->setTitle(tr("&Subtitles"));
	_menuSubtitles->setIcon(TkIcon("format-text-underline"));

	_menuTitles->setTitle(tr("&Title"));
	_menuTitles->setIcon(TkIcon("format-list-ordered"));

	_menuChapters->setTitle(tr("&Chapter"));
	_menuChapters->setIcon(TkIcon("x-office-address-book"));

	_menuAngles->setTitle(tr("&Angle"));
	_menuFile->setTitle(tr("&File"));
	_menuAudio->setTitle(tr("&Audio"));
	_menuSubtitle->setTitle(tr("&Subtitle"));
	_menuBrowse->setTitle(tr("&Browse"));
	_menuSettings->setTitle(tr("&Settings"));
	_menuHelp->setTitle(tr("&Help"));
}

QMenu * MainWindow::menuAudioChannels() const {
	return _menuAudioChannels;
}

QMenu * MainWindow::menuSubtitles() const {
	return _menuSubtitles;
}

QMenu * MainWindow::menuTitles() const {
	return _menuTitles;
}

QMenu * MainWindow::menuChapters() const {
	return _menuChapters;
}

QMenu * MainWindow::menuAngles() const {
	return _menuAngles;
}

void MainWindow::dragEnterEvent(QDragEnterEvent * event) {
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

			QFileInfo fileInfo(filename);

			qDebug() << __FUNCTION__ << "File suffix:" << fileInfo.suffix();

			bool isSubtitle = FileExtensions::subtitle().contains(fileInfo.suffix(), Qt::CaseInsensitive);
			if (isSubtitle) {
				qDebug() << __FUNCTION__ << "Loading subtitle:" << filename;
				emit subtitleFileDropped(filename);
			} else if (fileInfo.isDir()) {
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

void MainWindow::addBrowserDockWidget(QDockWidget * widget) {
	static QDockWidget * lastDockWidget = NULL;

	addDockWidget(Qt::LeftDockWidgetArea, widget);
	if (lastDockWidget) {
		tabifyDockWidget(lastDockWidget, widget);
	}
	lastDockWidget = widget;
}

void MainWindow::addVideoDockWidget(QDockWidget * widget) {
	static QDockWidget * lastDockWidget = NULL;

	addDockWidget(Qt::RightDockWidgetArea, widget);
	if (lastDockWidget) {
		tabifyDockWidget(lastDockWidget, widget);
	}
	lastDockWidget = widget;
}

void MainWindow::addPlaylistDockWidget(QDockWidget * widget) {
	static QDockWidget * lastDockWidget = NULL;

	addDockWidget(Qt::RightDockWidgetArea, widget);
	if (lastDockWidget) {
		tabifyDockWidget(lastDockWidget, widget);
	}
	lastDockWidget = widget;
}

void MainWindow::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	connect(mediaObject, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));

	disconnect(ActionCollection::action("quit"), SIGNAL(triggered()), mediaObject, SLOT(stop()));
	connect(ActionCollection::action("quit"), SIGNAL(triggered()), mediaObject, SLOT(stop()));
}
