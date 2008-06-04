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

#include "ActionCollection.h"
#include "MyIcon.h"
#include "PlayToolBar.h"
#include "VideoWidget.h"
#include "StatusBar.h"
#include "MediaController.h"
#include "QuickSettingsWindow.h"
#include "config/Config.h"
#include "config/ConfigWindow.h"

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/mediasource.h>

#include <QtCore/QSignalMapper>

#include <QtGui/QtGui>

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent) {

	populateActionCollection();

	setupUi();

	addRecentFilesToMenu();

	_mediaObject = new Phonon::MediaObject(this);
	_mediaObject->setTickInterval(1000);
	connect(_mediaObject, SIGNAL(metaDataChanged()),
		SLOT(metaDataChanged()));
	connect(_mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
		SLOT(sourceChanged(const Phonon::MediaSource &)));
	connect(_mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State, Phonon::State)));
	connect(_mediaObject, SIGNAL(aboutToFinish()), SLOT(aboutToFinish()));

	MediaController * mediaController = new MediaController(this, _mediaObject, this);

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

	//statusBar
	setStatusBar(new StatusBar(_mediaObject));

	connect(ActionCollection::action("playDVD"), SIGNAL(triggered()), SLOT(playDVD()));
	connect(ActionCollection::action("playURL"), SIGNAL(triggered()), SLOT(playURL()));
	connect(ActionCollection::action("playFile"), SIGNAL(triggered()), SLOT(playFile()));
	connect(ActionCollection::action("openSubtitleFile"), SIGNAL(triggered()), SLOT(openSubtitleFile()));
	connect(ActionCollection::action("equalizer"), SIGNAL(triggered()), SLOT(showQuickSettingsWindow()));
	connect(ActionCollection::action("preferences"), SIGNAL(triggered()), SLOT(showConfigWindow()));
	connect(ActionCollection::action("exit"), SIGNAL(triggered()), _mediaObject, SLOT(stop()));
	connect(ActionCollection::action("exit"), SIGNAL(triggered()), SLOT(close()));
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
			QFileInfo file(recentFiles[i]);
			QAction * action = _menuRecentFiles->addAction(file.completeBaseName(), signalMapper, SLOT(map()));
			signalMapper->setMapping(action, i);
		}

		_menuRecentFiles->addSeparator();
		_menuRecentFiles->addAction(ActionCollection::action("clearRecentFiles"));

		connect(signalMapper, SIGNAL(mapped(int)),
			SLOT(playRecentFile(int)));
	}
}

void MainWindow::playRecentFile(int id) {
	QStringList recentFiles = Config::instance().recentFiles();
	play(recentFiles[id]);
}

void MainWindow::clearRecentFiles() {
	//Clear recent files menu
	_menuRecentFiles->clear();
	_menuRecentFiles->addSeparator();
	_menuRecentFiles->addAction(ActionCollection::action("clearRecentFiles"));

	//Clear recent files configuration
	Config::instance().setValue(Config::RECENT_FILES_KEY, QStringList());
}

void MainWindow::playFile() {
	static const int MAX_RECENT_FILES = 10;

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Audio/Video File"));

	if (fileName.isEmpty()) {
		return;
	}

	QStringList recentFiles = Config::instance().recentFiles();
	bool prepend = false;
	if (recentFiles.isEmpty()) {
		prepend = true;
	} else if (fileName != recentFiles.first()) {
		prepend = true;
	}
	if (prepend) {
		recentFiles.prepend(fileName);
		if (recentFiles.size() > MAX_RECENT_FILES) {
			recentFiles.removeLast();
		}
	}

	Config::instance().setValue(Config::RECENT_FILES_KEY, recentFiles);

	addRecentFilesToMenu();

	play(fileName);
}

void MainWindow::playDVD() {
	play(Phonon::MediaSource(Phonon::Dvd, "D:"));
}

void MainWindow::playURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));
	if (!url.isEmpty()) {
		play(url);
	}
}

void MainWindow::play(const Phonon::MediaSource & mediaSource) {
	_mediaObject->setCurrentSource(mediaSource);
	_mediaObject->play();
}

void MainWindow::sourceChanged(const Phonon::MediaSource & source) {
	//currentTimeLcdNumber->display("00:00:00");
}

void MainWindow::metaDataChanged() {
	qDebug() << __FUNCTION__;

	QMultiMap<QString, QString> metaData = _mediaObject->metaData();

	QString title = metaData.value("TITLE");
	if (title.isEmpty()) {
		title = _mediaObject->currentSource().fileName();
	}

	/*titleLabel->setText(title);
	artistLabel->setText(metaData.value("ARTIST"));
	albumLabel->setText(metaData.value("ALBUM"));
	yearLabel->setText(metaData.value("DATE"));*/

	setWindowTitle(title + " - QuarkPlayer");
}

void MainWindow::stateChanged(Phonon::State newState, Phonon::State oldState) {
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
			_stackedWidget->setCurrentWidget(_backgroundLogoWidget);

			resize(minimumSize());
		}
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
	QuickSettingsWindow * quickSettings = new QuickSettingsWindow(_videoWidget, *_audioOutput, _audioOutputPath, *_mediaObject, this);
}

void MainWindow::about() {
	QMessageBox::information(this, tr("About QuarkPlayer"),
			tr(
			"QuarkPlayer, a Phonon media player\n" \
			"Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>\n\n" \
			"This program is free software: you can redistribute it and/or modify\n" \
			"it under the terms of the GNU General Public License as published by\n" \
			"the Free Software Foundation, either version 3 of the License, or\n" \
			"(at your option) any later version."
			));
}

void MainWindow::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("playFile", new QAction(app));
	ActionCollection::addAction("exit", new QAction(app));
	ActionCollection::addAction("about", new QAction(app));
	ActionCollection::addAction("aboutQt", new QAction(app));
	ActionCollection::addAction("playDVD", new QAction(app));
	ActionCollection::addAction("playURL", new QAction(app));
	ActionCollection::addAction("playVCD", new QAction(app));
	ActionCollection::addAction("equalizer", new QAction(app));
	ActionCollection::addAction("preferences", new QAction(app));
	ActionCollection::addAction("openSubtitleFile", new QAction(app));
	ActionCollection::addAction("clearRecentFiles", new QAction(app));
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
	_menuFile->addAction(ActionCollection::action("exit"));

	_menuAudio = new QMenu();
	menuBar()->addAction(_menuAudio->menuAction());
	_menuAudioChannels = new QMenu();
	_menuAudio->addAction(_menuAudioChannels->menuAction());

	_menuSubtitle = new QMenu();
	menuBar()->addAction(_menuSubtitle->menuAction());
	_menuSubtitle->addAction(ActionCollection::action("openSubtitleFile"));
	_menuSubtitles = new QMenu();
	_menuSubtitle->addAction(_menuSubtitles->menuAction());

	_menuBrowse = new QMenu();
	menuBar()->addAction(_menuBrowse->menuAction());
	_menuTitles = new QMenu();
	_menuBrowse->addAction(_menuTitles->menuAction());
	_menuChapters = new QMenu();
	_menuBrowse->addAction(_menuChapters->menuAction());
	_menuAngles = new QMenu();
	_menuBrowse->addAction(_menuAngles->menuAction());

	_menuOptions = new QMenu();
	menuBar()->addAction(_menuOptions->menuAction());
	_menuOptions->addAction(ActionCollection::action("equalizer"));
	_menuOptions->addAction(ActionCollection::action("preferences"));

	_menuHelp = new QMenu();
	menuBar()->addAction(_menuHelp->menuAction());
	_menuHelp->addAction(ActionCollection::action("about"));
	_menuHelp->addAction(ActionCollection::action("aboutQt"));

	//Main ToolBar
	_mainToolBar = new QToolBar();
	_mainToolBar->addAction(ActionCollection::action("playFile"));
	_mainToolBar->addAction(ActionCollection::action("playDVD"));
	_mainToolBar->addAction(ActionCollection::action("playURL"));
	_mainToolBar->addSeparator();
	_mainToolBar->addAction(ActionCollection::action("equalizer"));
	_mainToolBar->addAction(ActionCollection::action("preferences"));
	_mainToolBar->addSeparator();
	_mainToolBar->addAction(ActionCollection::action("exit"));
	addToolBar(_mainToolBar);

	retranslate();
}

void MainWindow::changeEvent(QEvent * event) {
	if (event->type() == QEvent::LanguageChange) {
		retranslate();
	} else {
		QMainWindow::changeEvent(event);
	}
}

void MainWindow::retranslate() {
	setWindowTitle(tr("QuarkPlayer"));
	setWindowIcon(QIcon(":/icons/quarkplayer.png"));

	ActionCollection::action("playFile")->setText(tr("Play File..."));
	ActionCollection::action("playFile")->setIcon(MyIcon("document-open"));

	ActionCollection::action("exit")->setText(tr("Exit"));
	ActionCollection::action("exit")->setIcon(MyIcon("application-exit"));

	ActionCollection::action("about")->setText(tr("About"));
	ActionCollection::action("about")->setIcon(MyIcon("help-about"));

	ActionCollection::action("aboutQt")->setText(tr("About Qt"));
	ActionCollection::action("aboutQt")->setIcon(MyIcon("help-about"));

	ActionCollection::action("playDVD")->setText(tr("Play DVD"));
	ActionCollection::action("playDVD")->setIcon(MyIcon("media-optical"));

	ActionCollection::action("playURL")->setText(tr("Play URL..."));
	ActionCollection::action("playURL")->setIcon(MyIcon("document-open-remote"));

	ActionCollection::action("playVCD")->setText(tr("Play VCD"));
	ActionCollection::action("playVCD")->setIcon(MyIcon("media-optical"));

	ActionCollection::action("equalizer")->setText(tr("Equalizer"));
	ActionCollection::action("equalizer")->setIcon(MyIcon("view-media-equalizer"));

	ActionCollection::action("preferences")->setText(tr("Preferences"));
	ActionCollection::action("preferences")->setIcon(MyIcon("preferences-system"));

	ActionCollection::action("openSubtitleFile")->setText(tr("Open Subtitle..."));
	ActionCollection::action("openSubtitleFile")->setIcon(MyIcon("document-open"));

	ActionCollection::action("clearRecentFiles")->setText(tr("Clear"));
	ActionCollection::action("clearRecentFiles")->setIcon(MyIcon("edit-delete"));

	_mainToolBar->setWindowTitle(tr("Main ToolBar"));

	_menuRecentFiles->setTitle(tr("Recent Files"));
	_menuRecentFiles->setIcon(MyIcon("document-open-recent"));

	_menuAudioChannels->setTitle(tr("Audio Channels"));
	_menuAudioChannels->setIcon(MyIcon("audio-x-generic"));

	_menuSubtitles->setTitle(tr("Subtitles"));
	_menuSubtitles->setIcon(MyIcon("format-text-underline"));

	_menuTitles->setTitle(tr("Title"));
	_menuTitles->setIcon(MyIcon("format-list-ordered"));

	_menuChapters->setTitle(tr("Chapter"));
	_menuChapters->setIcon(MyIcon("x-office-address-book"));

	_menuAngles->setTitle(tr("Angle"));
	_menuFile->setTitle(tr("File"));
	_menuAudio->setTitle(tr("Audio"));
	_menuSubtitle->setTitle(tr("Subtitle"));
	_menuBrowse->setTitle(tr("Browse"));
	_menuOptions->setTitle(tr("Options"));
	_menuHelp->setTitle(tr("Help"));
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
