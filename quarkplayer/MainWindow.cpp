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

	connect(ActionCollection::action("actionPlayDVD"), SIGNAL(triggered()), SLOT(playDVD()));
	connect(ActionCollection::action("actionPlayURL"), SIGNAL(triggered()), SLOT(playURL()));
	connect(ActionCollection::action("actionPlayFile"), SIGNAL(triggered()), SLOT(playFile()));
	connect(ActionCollection::action("actionOpenSubtitleFile"), SIGNAL(triggered()), SLOT(openSubtitleFile()));
	connect(ActionCollection::action("actionEqualizer"), SIGNAL(triggered()), SLOT(showQuickSettingsWindow()));
	connect(ActionCollection::action("actionPreferences"), SIGNAL(triggered()), SLOT(showConfigWindow()));
	connect(ActionCollection::action("actionExit"), SIGNAL(triggered()), _mediaObject, SLOT(stop()));
	connect(ActionCollection::action("actionExit"), SIGNAL(triggered()), SLOT(close()));
	connect(ActionCollection::action("actionAbout"), SIGNAL(triggered()), SLOT(about()));
	connect(ActionCollection::action("actionAboutQt"), SIGNAL(triggered()), qApp, SLOT(aboutQt()));
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
	connect(ActionCollection::action("actionClearRecentFiles"), SIGNAL(triggered()), SLOT(clearRecentFiles()));

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
		_menuRecentFiles->addAction(ActionCollection::action("actionClearRecentFiles"));

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
	_menuRecentFiles->addAction(ActionCollection::action("actionClearRecentFiles"));

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
	QAction * action;

	action = new QAction(app);
	action->setText(tr("Play File..."));
	action->setIcon(MyIcon("document-open"));
	ActionCollection::addAction("actionPlayFile", action);

	action = new QAction(app);
	action->setText(tr("Exit"));
	action->setIcon(MyIcon("application-exit"));
	ActionCollection::addAction("actionExit", action);

	action = new QAction(app);
	action->setText(tr("View Equalizer"));
	action->setIcon(MyIcon("view-media-equalizer"));
	ActionCollection::addAction("actionViewEqualizer", action);

	action = new QAction(app);
	action->setText(tr("About"));
	action->setIcon(MyIcon("help-about"));
	ActionCollection::addAction("actionAbout", action);

	action = new QAction(app);
	action->setText(tr("About Qt"));
	action->setIcon(MyIcon("help-about"));
	ActionCollection::addAction("actionAboutQt", action);

	action = new QAction(app);
	action->setText(tr("Play DVD"));
	action->setIcon(MyIcon("media-optical"));
	ActionCollection::addAction("actionPlayDVD", action);

	action = new QAction(app);
	action->setText(tr("Play URL..."));
	action->setIcon(MyIcon("document-open-remote"));
	ActionCollection::addAction("actionPlayURL", action);

	action = new QAction(app);
	action->setText(tr("Play VCD"));
	action->setIcon(MyIcon("media-optical"));
	ActionCollection::addAction("actionPlayVCD", action);

	action = new QAction(app);
	action->setText(tr("Equalizer"));
	action->setIcon(MyIcon("view-media-equalizer"));
	ActionCollection::addAction("actionEqualizer", action);

	action = new QAction(app);
	action->setText(tr("Preferences"));
	action->setIcon(MyIcon("preferences-system"));
	ActionCollection::addAction("actionPreferences", action);

	action = new QAction(app);
	action->setText(tr("Open Subtitle..."));
	action->setIcon(MyIcon("document-open"));
	ActionCollection::addAction("actionOpenSubtitleFile", action);

	action = new QAction(app);
	action->setText(tr("Clear"));
	action->setIcon(MyIcon("edit-delete"));
	ActionCollection::addAction("actionClearRecentFiles", action);
}

void MainWindow::setupUi() {
	setWindowTitle(tr("QuarkPlayer"));
	setWindowIcon(QIcon(":/icons/quarkplayer.png"));

	_stackedWidget = new QStackedWidget();
	setCentralWidget(_stackedWidget);

	QMenu * menuFile = menuBar()->addMenu(tr("File"));
	menuFile->addAction(ActionCollection::action("actionPlayFile"));

	_menuRecentFiles = new QMenu(tr("Recent Files"));
	_menuRecentFiles->setIcon(MyIcon("document-open-recent"));
	menuFile->addMenu(_menuRecentFiles);

	menuFile->addAction(ActionCollection::action("actionPlayDVD"));
	menuFile->addAction(ActionCollection::action("actionPlayURL"));
	menuFile->addAction(ActionCollection::action("actionPlayVCD"));
	menuFile->addSeparator();
	menuFile->addAction(ActionCollection::action("actionExit"));

	QMenu * menuAudio = menuBar()->addMenu(tr("Audio"));
	_menuAudioChannels = new QMenu(tr("Audio Channels"));
	_menuAudioChannels->setIcon(MyIcon("audio-x-generic"));
	menuAudio->addMenu(_menuAudioChannels);

	QMenu * menuSubtitle = menuBar()->addMenu(tr("Subtitle"));
	menuSubtitle->addAction(ActionCollection::action("actionOpenSubtitleFile"));
	_menuSubtitles = new QMenu(tr("Subtitles"));
	_menuSubtitles->setIcon(MyIcon("format-text-underline"));
	menuSubtitle->addMenu(_menuSubtitles);

	QMenu * menuBrowse = menuBar()->addMenu(tr("Browse"));
	_menuTitles = new QMenu(tr("Title"));
	_menuTitles->setIcon(MyIcon("format-list-ordered"));
	menuBrowse->addMenu(_menuTitles);
	_menuChapters = new QMenu(tr("Chapter"));
	_menuChapters->setIcon(MyIcon("x-office-address-book"));
	menuBrowse->addMenu(_menuChapters);
	_menuAngles = new QMenu(tr("Angle"));
	menuBrowse->addMenu(_menuAngles);

	QMenu * menuOptions = menuBar()->addMenu(tr("Options"));
	menuOptions->addAction(ActionCollection::action("actionQuickSettings"));
	menuOptions->addAction(ActionCollection::action("actionPreferences"));

	QMenu * menuHelp = menuBar()->addMenu(tr("Help"));
	menuHelp->addAction(ActionCollection::action("actionAbout"));
	menuHelp->addAction(ActionCollection::action("actionAboutQt"));

	//mainToolBar
	QToolBar * mainToolBar = new QToolBar(tr("Main ToolBar"));
	mainToolBar->addAction(ActionCollection::action("actionPlayFile"));
	mainToolBar->addAction(ActionCollection::action("actionPlayDVD"));
	mainToolBar->addAction(ActionCollection::action("actionPlayURL"));
	mainToolBar->addSeparator();
	mainToolBar->addAction(ActionCollection::action("actionEqualizer"));
	mainToolBar->addAction(ActionCollection::action("actionPreferences"));
	mainToolBar->addSeparator();
	mainToolBar->addAction(ActionCollection::action("actionExit"));
	addToolBar(mainToolBar);
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
