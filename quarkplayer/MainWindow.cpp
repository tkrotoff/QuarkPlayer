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

#include "ui_BackgroundLogoWidget.h"

#include "PlayToolBar.h"
#include "VideoWidget.h"
#include "MediaDataWidget.h"
#include "StatusBar.h"
#include "MediaController.h"
#include "QuickSettingsWindow.h"
#include "AboutWindow.h"
#include "config/Config.h"
#include "config/ConfigWindow.h"
#include "version.h"

#include <tkutil/ActionCollection.h>
#include <tkutil/TkIcon.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/mediasource.h>

#include <QtCore/QSignalMapper>
#include <QtCore/QCoreApplication>

#include <QtGui/QtGui>

MainWindow::MainWindow(QWidget * parent)
	: TkMainWindow(parent) {

	populateActionCollection();

	setupUi();

	RETRANSLATE(this);
	retranslate();

	addRecentFilesToMenu();

	_mediaObject = new Phonon::MediaObject(this);
	_mediaObject->setTickInterval(1000);
	connect(_mediaObject, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
	connect(_mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
		SLOT(sourceChanged(const Phonon::MediaSource &)));
	connect(_mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State, Phonon::State)));
	connect(_mediaObject, SIGNAL(aboutToFinish()), SLOT(aboutToFinish()));
	connect(_mediaObject, SIGNAL(hasVideoChanged(bool)), SLOT(hasVideoChanged(bool)));

	MediaController * mediaController = new MediaController(this, _mediaObject);

	//audioOutput
	_audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	_audioOutputPath = Phonon::createPath(_mediaObject, _audioOutput);

	//toolBar
	_playToolBar = new PlayToolBar(_mediaObject, _audioOutput);

	//Logo widget
	_backgroundLogoWidget = new QWidget();
	_stackedWidget->addWidget(_backgroundLogoWidget);
	Ui::BackgroundLogoWidget * logo = new Ui::BackgroundLogoWidget();
	logo->setupUi(_backgroundLogoWidget);
	_stackedWidget->setCurrentWidget(_backgroundLogoWidget);

	//videoWidget
	_videoWidget = new VideoWidget(this, _mediaObject);
	Phonon::createPath(_mediaObject, _videoWidget);
	_stackedWidget->addWidget(_videoWidget);

	//mediaDataWidget
	_mediaDataWidget = new MediaDataWidget(*_mediaObject);
	_stackedWidget->addWidget(_mediaDataWidget);

	//statusBar
	setStatusBar(new StatusBar(_mediaObject));

	connect(ActionCollection::action("playDVD"), SIGNAL(triggered()), SLOT(playDVD()));
	connect(ActionCollection::action("playURL"), SIGNAL(triggered()), SLOT(playURL()));
	connect(ActionCollection::action("playFile"), SIGNAL(triggered()), SLOT(playFile()));
	connect(ActionCollection::action("equalizer"), SIGNAL(triggered()), SLOT(showQuickSettingsWindow()));
	connect(ActionCollection::action("configure"), SIGNAL(triggered()), SLOT(showConfigWindow()));
	connect(ActionCollection::action("quit"), SIGNAL(triggered()), _mediaObject, SLOT(stop()));
	connect(ActionCollection::action("quit"), SIGNAL(triggered()), SLOT(close()));
	connect(ActionCollection::action("about"), SIGNAL(triggered()), SLOT(about()));
	connect(ActionCollection::action("aboutQt"), SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

MainWindow::~MainWindow() {
}

PlayToolBar * MainWindow::playToolBar() const {
	return _playToolBar;
}

VideoWidget * MainWindow::videoWidget() const {
	return _videoWidget;
}

QStackedWidget * MainWindow::stackedWidget() const {
	return _stackedWidget;
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
	QString filename = TkFileDialog::getOpenFileName(this, tr("Select Audio/Video File"));

	if (!filename.isEmpty()) {
		play(filename);
	}
}

void MainWindow::playDVD() {
	play(Phonon::MediaSource(Phonon::Dvd, "D:"));
}

void MainWindow::playURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));
	qDebug() << __FUNCTION__ << "url:" << url;

	if (!url.isEmpty()) {
		play(url);
	}
}

void MainWindow::play(const Phonon::MediaSource & mediaSource) {
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

	_mediaObject->setCurrentSource(mediaSource);
	_mediaObject->play();
}

void MainWindow::sourceChanged(const Phonon::MediaSource & source) {
	//currentTimeLcdNumber->display("00:00:00");
}

void MainWindow::metaDataChanged() {
	qDebug() << __FUNCTION__;

	QMultiMap<QString, QString> metaData = _mediaObject->metaData();

	QString windowTitle;
	QString artist = metaData.value("ARTIST");
	QString title = metaData.value("TITLE");
	if (artist.isEmpty() && title.isEmpty()) {
		windowTitle = _mediaObject->currentSource().fileName();
		windowTitle = windowTitle.right(windowTitle.length() - windowTitle.lastIndexOf('/') - 1);
	} else {
		if (!title.isEmpty()) {
			windowTitle = title;
		}
		if (!artist.isEmpty()) {
			if (!windowTitle.isEmpty()) {
				windowTitle += " - ";
			}
			windowTitle += artist;
		}
	}

	if (windowTitle.isEmpty()) {
		setWindowTitle(QCoreApplication::applicationName());
	} else {
		setWindowTitle(windowTitle + " - " + QCoreApplication::applicationName() + " ");
	}
}

void MainWindow::stateChanged(Phonon::State newState, Phonon::State oldState) {
	/*
	//Remove the background logo, not needed anymore
	if (_backgroundLogoWidget) {
		_stackedWidget->removeWidget(_backgroundLogoWidget);
		delete _backgroundLogoWidget;
		_backgroundLogoWidget = NULL;
	}

	if (oldState == Phonon::LoadingState) {
		//Resize the main window to the size of the video
		//i.e increase or decrease main window size if needed
		if (_mediaObject->hasVideo()) {
			_stackedWidget->setCurrentWidget(_videoWidget);

			//Flush event so that sizeHint takes the
			//recently shown/hidden _videoWidget into account
			QApplication::instance()->processEvents();
			resize(sizeHint());
		} else {
			_stackedWidget->setCurrentWidget(_mediaDataWidget);

			//Flush event so that sizeHint takes the
			//recently shown/hidden _videoWidget into account
			QApplication::instance()->processEvents();
			resize(minimumSize());
		}
	}
	*/
}

void MainWindow::hasVideoChanged(bool hasVideo) {
	//Remove the background logo, not needed anymore
	if (_backgroundLogoWidget) {
		_stackedWidget->removeWidget(_backgroundLogoWidget);
		delete _backgroundLogoWidget;
		_backgroundLogoWidget = NULL;
	}

	//Resize the main window to the size of the video
	//i.e increase or decrease main window size if needed
	if (hasVideo) {
		_stackedWidget->setCurrentWidget(_videoWidget);

		//Flush event so that sizeHint takes the
		//recently shown/hidden _videoWidget into account
		QApplication::instance()->processEvents();
		resize(sizeHint());
	} else {
		_stackedWidget->setCurrentWidget(_mediaDataWidget);

		//Flush event so that sizeHint takes the
		//recently shown/hidden _videoWidget into account
		QApplication::instance()->processEvents();
		resize(minimumSize());
	}
}

void MainWindow::aboutToFinish() {
}

void MainWindow::closeEvent(QCloseEvent * event) {
	_mediaObject->stop();
	event->accept();
}

void MainWindow::showConfigWindow() {
	static ConfigWindow * configWindow = new ConfigWindow(this);
	configWindow->show();
}

void MainWindow::showQuickSettingsWindow() {
	static QuickSettingsWindow * quickSettings = new QuickSettingsWindow(_videoWidget, *_audioOutput, _audioOutputPath, *_mediaObject, this);
	quickSettings->show();
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
	_stackedWidget = new QStackedWidget();
	setCentralWidget(_stackedWidget);

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
