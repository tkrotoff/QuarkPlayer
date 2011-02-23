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

#include "MainWindow.h"

#include "CommonActions.h"
#include "AboutWindow.h"
#include "MainWindowLogger.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/PluginManager.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/version.h>

#include <quarkplayer-plugins/Playlist/PlaylistWidget.h>
#include <quarkplayer-plugins/Playlist/PlaylistModel.h>

#include <Logger/LogWindow.h>

#include <TkUtil/Actions.h>
#include <TkUtil/TkFileDialog.h>
#include <TkUtil/TkToolBar.h>
#include <TkUtil/LanguageChangeEventFilter.h>

#include <FileTypes/FileTypes.h>

#include <QtSingleApplication>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>
#include <phonon/audiooutput.h>

#include <QtGui/QtGui>

Q_EXPORT_PLUGIN2(MainWindow, MainWindowFactory);

const char * MainWindowFactory::PLUGIN_NAME = "MainWindow";

QStringList MainWindowFactory::dependencies() const {
	QStringList tmp;
	return tmp;
}

PluginInterface * MainWindowFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new MainWindow(quarkPlayer, uuid);
}

MainWindow * MainWindowFactory::mainWindow() {
	MainWindow * mainWindow = dynamic_cast<MainWindow *>(PluginManager::instance().pluginInterface(PLUGIN_NAME));
	Q_ASSERT(mainWindow);
	return mainWindow;
}

MainWindow::MainWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: IMainWindow(quarkPlayer, uuid) {

	new CommonActions(quarkPlayer, this);

	QtSingleApplication * app = qobject_cast<QtSingleApplication *>(QCoreApplication::instance());
	if (app) {
		app->setActivationWindow(this);
		app->activateWindow();
	}

	populateActionCollection();

	setupUi();

	//Accepts Drag&Drop
	setAcceptDrops(true);

	//DockWidgets tabs are vertical like in Amarok
	setDockOptions(QMainWindow::VerticalTabs);

	_playToolBar = NULL;
	_statusBar = NULL;

	connect(Actions["CommonActions.OpenFile"], SIGNAL(triggered()), SLOT(playFile()));
	connect(Actions["CommonActions.OpenDVD"], SIGNAL(triggered()), SLOT(playDVD()));
	connect(Actions["CommonActions.OpenURL"], SIGNAL(triggered()), SLOT(playURL()));
	connect(Actions["CommonActions.OpenVCD"], SIGNAL(triggered()), SLOT(playVCD()));
	connect(Actions["CommonActions.NewMediaObject"], SIGNAL(triggered()), &quarkPlayer, SLOT(createNewMediaObject()));
	connect(Actions["CommonActions.Quit"], SIGNAL(triggered()), SLOT(close()));
	connect(Actions["CommonActions.ReportBug"], SIGNAL(triggered()), SLOT(reportBug()));
	connect(Actions["CommonActions.ShowMailingList"], SIGNAL(triggered()), SLOT(showMailingList()));
	connect(Actions["CommonActions.ShowLog"], SIGNAL(triggered()), SLOT(showLog()));
	connect(Actions["CommonActions.About"], SIGNAL(triggered()), SLOT(about()));
	connect(Actions["CommonActions.AboutQt"], SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(Actions["CommonActions.VolumeMute"], SIGNAL(toggled(bool)), SLOT(mutedToggled(bool)));

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	bool ok = restoreGeometry(Config::instance().mainWindowGeometry());
	if (!ok) {
		MainWindowCritical() << "Couldn't restore main window geometry";
	}

	RETRANSLATE(this);
	retranslate();

	show();
}

MainWindow::~MainWindow() {
	//If the MainWindow is destroyed, it's reasonable to say
	//that we can quit the entire application
	QCoreApplication::quit();
}

void MainWindow::setPlayToolBar(QToolBar * playToolBar) {
	_playToolBar = playToolBar;

#ifdef Q_WS_MAC
	_playToolBar->setFloatable(false);
	_playToolBar->setMovable(false);
#endif	//!Q_WS_MAC

	addToolBar(Qt::BottomToolBarArea, playToolBar);

	emit playToolBarAdded(_playToolBar);
}

QToolBar * MainWindow::playToolBar() const {
	return _playToolBar;
}

QToolBar * MainWindow::mainToolBar() const {
	return _mainToolBar;
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

void MainWindow::playFile() {
	QStringList fileNames = TkFileDialog::getOpenFileNames(
		this, tr("Select Audio/Video File"), Config::instance().lastDirOpened(),
		tr("Multimedia") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video, FileType::Audio)) + ";;" +
		tr("Video") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video)) +";;" +
		tr("Audio") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Audio)) +";;" +
		tr("Playlist") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Playlist)) + ";;" +
		tr("All Files") + " (*)"
	);

	if (!fileNames.isEmpty()) {
		QString fileToPlay(fileNames[0]);
		Config::instance().setValue(Config::LAST_DIR_OPENED_KEY, QFileInfo(fileToPlay).absolutePath());

		PlaylistWidget * playlistWidget = PlaylistWidgetFactory::playlistWidget();
		if (playlistWidget) {
			playlistWidget->addFilesToCurrentPlaylist(fileNames);
			playlistWidget->playlistModel()->play(0);
		} else {
			play(fileToPlay);
		}
	}
}

QString MainWindow::getDiscPath(const QString & defaultPath, const QString & windowTitle) {
	QString path;

#ifdef Q_WS_WIN
	path = TkFileDialog::getExistingDirectory(this, windowTitle, defaultPath);
#else
	//Under Linux
	//FIXME and other OS like Mac OS X??
	path = TkFileDialog::getOpenFileName(this, windowTitle, defaultPath);
#endif	//Q_WS_WIN

	return path;
}

void MainWindow::playDVD() {
	QString dvd = getDiscPath(Config::instance().dvdPath(), tr("Select DVD Device"));
	if (!dvd.isEmpty()) {
		Config::instance().setValue(Config::DVD_PATH_KEY, dvd);
		play(Phonon::MediaSource(Phonon::Dvd, dvd));
	}
}

void MainWindow::playVCD() {
	QString cdrom = getDiscPath(Config::instance().cdromPath(), tr("Select SVCD/VCD Device"));
	if (!cdrom.isEmpty()) {
		Config::instance().setValue(Config::CDROM_PATH_KEY, cdrom);
		play(Phonon::MediaSource(Phonon::Vcd, cdrom));
	}
}

void MainWindow::playURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));

	if (!url.isEmpty()) {
		play(url);
	}
}

void MainWindow::play(const Phonon::MediaSource & mediaSource) {
	quarkPlayer().play(mediaSource);
	//_statusBar->showMessage(tr("Processing") + " " + fileName + "...");
}

void MainWindow::updateWindowTitle() {
	QString title = quarkPlayer().currentMediaObjectTitle();

	if (title.isEmpty()) {
		setWindowTitle(QCoreApplication::applicationName() + " " + QString(QUARKPLAYER_VERSION));
	} else {
		setWindowTitle(title);
	}
}

void MainWindow::reportBug() {
	QDesktopServices::openUrl(QUrl("http://code.google.com/p/quarkplayer/issues/list"));
}

void MainWindow::showMailingList() {
	QDesktopServices::openUrl(QUrl("http://groups.google.com/group/quarkplayer"));
}

void MainWindow::showLog() {
	static LogWindow * logWindow = new LogWindow(this);
	logWindow->show();
}

void MainWindow::about() {
	static AboutWindow * aboutWindow = new AboutWindow(this);
	aboutWindow->exec();
}

void MainWindow::populateActionCollection() {
}

void MainWindow::setupUi() {
	//No central widget, only QDockWidget
	//setCentralWidget(NULL);
	/*QWidget * centralWidget = new QWidget();
	centralWidget->setMaximumSize(0, 0);
	centralWidget->setSizeIncrement(0, 0);
	centralWidget->setBaseSize(0, 0);
	centralWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	centralWidget->setMinimumSize(0, 0);
	setCentralWidget(centralWidget);*/
	///

	_menuFile = new QMenu();
	menuBar()->addMenu(_menuFile);
	_menuFile->addAction(Actions["CommonActions.OpenFile"]);
	_menuFile->addAction(Actions["CommonActions.OpenURL"]);
	_menuFile->addAction(Actions["CommonActions.OpenDVD"]);
	_menuFile->addAction(Actions["CommonActions.OpenVCD"]);
	_menuFile->addSeparator();
	_menuFile->addAction(Actions["CommonActions.Quit"]);

	_menuPlay = new QMenu();
	menuBar()->addMenu(_menuPlay);
	_menuPlay->addAction(Actions["CommonActions.PreviousTrack"]);
	_menuPlay->addAction(Actions["CommonActions.PlayPause"]);
	_menuPlay->addAction(Actions["CommonActions.Stop"]);
	_menuPlay->addAction(Actions["CommonActions.NextTrack"]);
	_menuPlay->addSeparator();
	_menuPlay->addAction(Actions["CommonActions.JumpBackward10s"]);
	_menuPlay->addAction(Actions["CommonActions.JumpBackward1min"]);
	_menuPlay->addAction(Actions["CommonActions.JumpBackward10min"]);
	_menuPlay->addSeparator();
	_menuPlay->addAction(Actions["CommonActions.JumpForward10s"]);
	_menuPlay->addAction(Actions["CommonActions.JumpForward1min"]);
	_menuPlay->addAction(Actions["CommonActions.JumpForward10min"]);
	_menuPlay->addSeparator();
	_menuPlay->addAction(Actions["CommonActions.SpeedDecrease10%"]);
	_menuPlay->addAction(Actions["CommonActions.SpeedIncrease10%"]);
	_menuPlay->addSeparator();
	_menuPlay->addAction(Actions["CommonActions.FullScreen"]);
	_menuPlay->addSeparator();
	_menuPlay->addAction(Actions["CommonActions.NewMediaObject"]);

	_menuAudio = new QMenu();
	menuBar()->addMenu(_menuAudio);
	_menuAudio->addAction(Actions["CommonActions.VolumeMute"]);
	_menuAudio->addAction(Actions["CommonActions.VolumeIncrease10%"]);
	_menuAudio->addAction(Actions["CommonActions.VolumeDecrease10%"]);

	_menuSettings = new QMenu();
	menuBar()->addMenu(_menuSettings);
	_menuSettings->addAction(Actions["CommonActions.Equalizer"]);
	_menuSettings->addAction(Actions["CommonActions.Configure"]);

	_menuHelp = new QMenu();
	menuBar()->addMenu(_menuHelp);
	_menuHelp->addAction(Actions["CommonActions.ShowMailingList"]);
	_menuHelp->addAction(Actions["CommonActions.ReportBug"]);
	_menuHelp->addAction(Actions["CommonActions.ShowLog"]);
	_menuHelp->addSeparator();
	_menuHelp->addAction(Actions["CommonActions.About"]);
	_menuHelp->addAction(Actions["CommonActions.AboutQt"]);

	//Main ToolBar
	_mainToolBar = new TkToolBar(this);
	TkToolBar::setToolButtonStyle(_mainToolBar);
	_mainToolBar->addAction(Actions["CommonActions.OpenFile"]);
	_mainToolBar->addAction(Actions["CommonActions.OpenDVD"]);
	//_mainToolBar->addAction(Actions["CommonActions.OpenURL"]);
	//_mainToolBar->addSeparator();
	//_mainToolBar->addAction(Actions["CommonActions.Equalizer"]);
	//_mainToolBar->addAction(Actions["CommonActions.Configure"]);
	addToolBar(_mainToolBar);

	//Main toolbar accessible but disabled by default
	_mainToolBar->hide();
}

void MainWindow::retranslate() {
	updateWindowTitle();
	setWindowIcon(QIcon(":/icons/quarkplayer-16x16.png"));

	//Main toolbar accessible but disabled by default
	_mainToolBar->setWindowTitle(tr("Main ToolBar"));
	_mainToolBar->setMinimumSize(_mainToolBar->sizeHint());

	_menuFile->setTitle(tr("&File"));
	_menuPlay->setTitle(tr("&Play"));
	_menuAudio->setTitle(tr("&Audio"));
	_menuSettings->setTitle(tr("&Settings"));
	_menuHelp->setTitle(tr("&Help"));
}

QMenu * MainWindow::menuFile() const {
	return _menuFile;
}

QMenu * MainWindow::menuPlay() const {
	return _menuPlay;
}

QMenu * MainWindow::menuAudio() const {
	return _menuAudio;
}

QMenu * MainWindow::menuSettings() const {
	return _menuSettings;
}

QMenu * MainWindow::menuHelp() const {
	return _menuHelp;
}

void MainWindow::dragEnterEvent(QDragEnterEvent * event) {
	MainWindowDebug() << event->mimeData()->formats();

	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent * event) {
	QStringList files;

	//Add urls to a list
	if (event->mimeData()->hasUrls()) {
		QList<QUrl> urlList = event->mimeData()->urls();
		QString fileName;
		foreach (QUrl url, urlList) {
			if (url.isValid()) {
				MainWindowDebug() << "File scheme:" << url.scheme();
				if (url.scheme() == "file") {
					fileName = url.toLocalFile();
				} else {
					fileName = url.toString();
				}
				MainWindowDebug() << "File dropped:" << fileName;
				files << fileName;
			}
		}
	}

	if (files.count() > 0) {
		if (files.count() == 1) {
			//1 file
			QString fileName = files[0];

			bool isSubtitle = FileTypes::fileExtensionMatches(fileName, FileTypes::extensions(FileType::Subtitle));
			if (isSubtitle) {
				MainWindowDebug() << "Loading subtitle:" << fileName;
				emit subtitleFileDropped(fileName);
			} else if (QFileInfo(fileName).isDir()) {
				//TODO open directory
			} else {
				//TODO add to playlist if 'auto-add-playlist' option
				play(fileName);
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

	event->accept();

	//Quits the application
	//QCoreApplication::quit();

	//FIXME we should only use QCoreApplication::quit()
	//exit(EXIT_SUCCESS);
}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget * lastDockWidget, QDockWidget * dockWidget) {
	if (dockWidget) {
		//dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);

#ifdef Q_WS_MAC
		//QDockWidgets cannot be floatable under Mac
		//so let's fully disable this feature
		dockWidget->setFloating(false);
#endif	//Q_WS_MAC

		//To hide the title bar completely
		//we must replace the default widget with a generic one
		dockWidget->setTitleBarWidget(new QWidget(this));

		QMainWindow::addDockWidget(area, dockWidget);
		if (lastDockWidget) {
			tabifyDockWidget(lastDockWidget, dockWidget);
		}
	}
}

void MainWindow::addBrowserDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::LeftDockWidgetArea, lastDockWidget, dockWidget);
	lastDockWidget = dockWidget;
}

void MainWindow::resetBrowserDockWidget() {
	addBrowserDockWidget(NULL);
}

void MainWindow::addVideoDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::RightDockWidgetArea, lastDockWidget, dockWidget);
	lastDockWidget = dockWidget;
}

void MainWindow::resetVideoDockWidget() {
	addVideoDockWidget(NULL);
}

void MainWindow::addPlaylistDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::RightDockWidgetArea, lastDockWidget, dockWidget);
	lastDockWidget = dockWidget;
}

void MainWindow::resetPlaylistDockWidget() {
	addPlaylistDockWidget(NULL);
}

void MainWindow::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	//Resets the window title when needed
	connect(mediaObject, SIGNAL(metaDataChanged()), SLOT(updateWindowTitle()));

	disconnect(Actions["CommonActions.Quit"], SIGNAL(triggered()), mediaObject, SLOT(stop()));
	connect(Actions["CommonActions.Quit"], SIGNAL(triggered()), mediaObject, SLOT(stop()));

	Phonon::AudioOutput * audioOutput = quarkPlayer().currentAudioOutput();
	if (audioOutput) {
		//Avoid a crash inside Phonon if the backend couldn't be loaded
		Actions["CommonActions.VolumeMute"]->setChecked(audioOutput->isMuted());
		disconnect(audioOutput, SIGNAL(mutedChanged(bool)), this, SLOT(mutedChanged(bool)));
		connect(audioOutput, SIGNAL(mutedChanged(bool)), SLOT(mutedChanged(bool)));
	}
}

void MainWindow::mutedChanged(bool muted) {
	Actions["CommonActions.VolumeMute"]->setChecked(muted);
}

void MainWindow::mutedToggled(bool muted) {
	Phonon::AudioOutput * audioOutput = quarkPlayer().currentAudioOutput();
	Q_ASSERT(audioOutput);
	audioOutput->setMuted(muted);
}
